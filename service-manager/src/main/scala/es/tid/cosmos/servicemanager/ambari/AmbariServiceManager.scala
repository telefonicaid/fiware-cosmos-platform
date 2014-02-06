/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.servicemanager.ambari

import java.net.URI
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.typesafe.scalalogging.slf4j.Logging

import es.tid.cosmos.common.PassThrough
import es.tid.cosmos.platform.ial.{MachineProfile, InfrastructureProvider, MachineState}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.rest.{AmbariServer, Service}
import es.tid.cosmos.servicemanager.ambari.services.ServiceDependencies._
import es.tid.cosmos.servicemanager.ambari.services.ServiceWithConfigurationFile._
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.util.TcpServer
import es.tid.cosmos.servicemanager.util.TcpServer._

/** Manager of the Ambari service configuration workflow.
  * It allows creating clusters with specified services using Ambari.
  *
  * @constructor
  * @param ambariServer              the cluster provisioner
  * @param infrastructureProvider    the host-machines provider
  * @param persistentHdfsId          the id of the persistent hdfs cluster
  * @param exclusiveMasterSizeCutoff the minimum size of a cluster such that
  *                                  the master node stops acting like a slave
  * @param hadoopConfig              parameters that fine-tune configuration of Hadoop
  * @param clusterDao                the dao that stores cluster states
  */
class AmbariServiceManager(
    ambariServer: AmbariServer,
    infrastructureProvider: InfrastructureProvider,
    override val persistentHdfsId: ClusterId,
    exclusiveMasterSizeCutoff: Int,
    hadoopConfig: HadoopConfig,
    clusterDao: AmbariClusterDao) extends ServiceManager with Logging {
  import AmbariServiceManager._

  private implicit val serviceConfigPath = hadoopConfig.servicesConfigDirectory
  
  private val dynamicProperties = new DynamicPropertiesFactory(hadoopConfig, () => for {
    description <- describePersistentHdfsCluster()
    master <- description.master
  } yield master.hostname)
  
  private[ambari] val clusterDeployer = new ClusterManager(
    ambariServer, serviceConfigPath, infrastructureProvider.rootPrivateSshKey)

  override def clusterIds: Seq[ClusterId] = clusterDao.ids

  override def describeCluster(id: ClusterId): Option[ImmutableClusterDescription] =
    clusterDao.getDescription(id).map(_.view)

  override val optionalServices: Seq[ServiceDescription] = OptionalServices

  private def userServices(users: Seq[ClusterUser]): Seq[ServiceDescription] =
    Seq(new CosmosUserService(users))

  override def createCluster(
      name: String,
      clusterSize: Int,
      serviceDescriptions: Seq[ServiceDescription],
      users: Seq[ClusterUser],
      preConditions: ClusterExecutableValidation): ClusterId = {
    val clusterServiceDescriptions =
      (BasicHadoopServices ++ serviceDescriptions ++ userServices(users))
        .withDependencies
        .map(decorateWithFileConfiguration)
    val clusterDescription = clusterDao.registerCluster(
      name = name,
      size = clusterSize,
      services = clusterServiceDescriptions.toSet
    )
    clusterDescription.withFailsafe {
      for {
        machines <- infrastructureProvider.createMachines(
          preConditions(clusterDescription.id), MachineProfile.G1Compute, clusterSize, waitForSsh)
        (master, slaves) = masterAndSlaves(machines)
        _ = setMachineInfo(clusterDescription, master, slaves)
        _ <- createCluster(clusterDescription, clusterServiceDescriptions)
        _ = clusterDao.setUsers(clusterDescription.id, users.toSet)
      } yield ()
    }
    clusterDescription.id
  }

  private def masterAndSlaves(machines: Seq[MachineState]) = machines match {
    case Seq(master, slaves @ _*) if machines.length < exclusiveMasterSizeCutoff =>
      (master, machines)
    case Seq(master, slaves @ _*) => (master, slaves)
    case _ => throw new IllegalArgumentException("Need at least one machine")
  }

  private def createCluster(
      dbClusterDescription: MutableClusterDescription,
      serviceDescriptions: Seq[AmbariServiceDescription]) = {
    require(dbClusterDescription.master.isDefined,
      "This function cannot be called if the master has not been set")
    require(dbClusterDescription.slaves.nonEmpty,
      "This function cannot be called if the slaves have not been set")
    val master = dbClusterDescription.master.get
    val deployment_> = for {
      _ <- clusterDeployer.deployCluster(
        dbClusterDescription.view, serviceDescriptions, dynamicProperties)
    } yield {
      dbClusterDescription.nameNode = toNameNodeUri(master)
      dbClusterDescription.state = Running
    }
    deployment_>
  }

  /** Create the namenode URI for the given host.
    *
    * @param host the host for whom to create the URI
    * @return the namenode URI e.g. `hdfs://localhost:50070`
    */
  private def toNameNodeUri(host: HostDetails) =
    new URI(s"hdfs://${host.hostname}:${hadoopConfig.nameNodeHttpPort}")

  override def terminateCluster(id: ClusterId): Future[Unit] = {
    val mutableCluster = clusterDao.getDescription(id).getOrElse(
      throw new IllegalArgumentException(s"Cluster $id does not exist"))
    mutableCluster.withFailsafe {
      val cluster = mutableCluster.view
      val clusterState = cluster.state
      require(clusterState.canTerminate,
        s"Cluster $id is in state $clusterState, which is not a valid state for termination")
      mutableCluster.state = Terminating
      for {
        _ <- clusterDeployer.removeClusterFromAmbari(cluster)
        machines <- infrastructureProvider.assignedMachines(cluster.machines.map(_.hostname))
        _ <- infrastructureProvider.releaseMachines(machines)
      } yield {
        mutableCluster.state = Terminated
      }
    }
  }

  override def listUsers(clusterId: ClusterId): Option[Seq[ClusterUser]] =
    clusterDao.getUsers(clusterId).map(_.toSeq)

  override def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] = {
    val clusterDescription = describeCluster(clusterId)
    require(
      clusterDescription.isDefined,
      s"Cluster with id [$clusterId] is not managed by this ServiceManager")
    require(
      clusterDescription.get.state == Running,
      s"Cluster[$clusterId] not Running")
    for {
      changedService <- changeServiceConfiguration(
        clusterId,
        clusterDescription.get,
        new CosmosUserService(clusterUsersDelta(listUsers(clusterId), users)))
    } yield {
      clusterDao.setUsers(clusterId, users.toSet)
    }
  }

  private def clusterUsersDelta(
      currentUsers: Option[Seq[ClusterUser]],
      newUsers: Seq[ClusterUser]): Seq[ClusterUser] =
    (newUsers.toSet diff currentUsers.toSet.flatten).toSeq

  private def changeServiceConfiguration(
      id: ClusterId,
      clusterDescription: ClusterDescription,
      serviceDescription: AmbariServiceDescription): Future[Service] = for {
    cluster <- ambariServer.getCluster(id.toString)
    service <- cluster.getService(serviceDescription.name)
    master <- ServiceMasterExtractor.getServiceMaster(cluster, serviceDescription)
    stoppedService <- service.stop()
    slaveDetails = clusterDescription.slaves
    slaves <- Future.traverse(slaveDetails)(details => cluster.getHost(details.hostname))
    _ <- Configurator.applyConfiguration(
      cluster,
      properties = dynamicProperties.forCluster(master.name, slaves.map(_.name)),
      contributors = List(serviceDescription)
    )
    startedService <- stoppedService.start()
  } yield startedService

  override def deployPersistentHdfsCluster(): Future[Unit] = for {
    machineCount <- infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave)
    serviceDescriptions = Seq(Zookeeper, Hdfs, new CosmosUserService(Seq()))
      .map(decorateWithFileConfiguration)
    clusterDescription = clusterDao.registerCluster(
      id = persistentHdfsId,
      name = persistentHdfsId.id,
      size = machineCount + 1,
      services = serviceDescriptions.toSet
    )
    _ <- clusterDescription.withFailsafe(for {
      master <- infrastructureProvider.createMachines(
        PassThrough, MachineProfile.HdfsMaster, numberOfMachines = 1, waitForSsh).map(_.head)
      slaves <- infrastructureProvider.createMachines(
        PassThrough, MachineProfile.HdfsSlave, numberOfMachines = machineCount, waitForSsh)
      _ = setMachineInfo(clusterDescription, master, slaves)
      _ <- createCluster(clusterDescription, serviceDescriptions)
    } yield ())
  } yield ()

  override def clusterNodePoolCount: Int =
    infrastructureProvider.machinePoolCount(_ == MachineProfile.G1Compute)
}

private[ambari] object AmbariServiceManager {
  val BasicHadoopServices: Seq[ServiceDescription] = Seq(Hdfs, MapReduce2, InfinityfsDriver)
  val OptionalServices: Seq[ServiceDescription] = Seq(Hive, Oozie, Pig, Sqoop)
  val AllServices: Seq[ServiceDescription] =
    (BasicHadoopServices ++ OptionalServices :+ CosmosUserService).withDependencies.distinct

  private def setMachineInfo(
      description: MutableClusterDescription, master: MachineState, slaves: Seq[MachineState]) {
    description.master = toHostInfo(master)
    description.slaves = slaves.map(toHostInfo)
  }

  private def toHostInfo(host: MachineState) = HostDetails(host.hostname, host.ipAddress)

  private def waitForSsh(state: MachineState): Future[Unit] =
    TcpServer(state.hostname, SshServicePort).waitForServer()
}
