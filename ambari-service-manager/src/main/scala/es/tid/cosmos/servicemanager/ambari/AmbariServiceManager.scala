/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager.ambari

import java.net.URI
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.typesafe.scalalogging.slf4j.Logging

import es.tid.cosmos.common.{NowFuture, PassThrough}
import es.tid.cosmos.platform.ial.{MachineProfile, InfrastructureProvider, MachineState}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.AmbariServiceManager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.util.TcpServerWatcher
import es.tid.cosmos.servicemanager.util.TcpServerWatcher._
import es.tid.cosmos.servicemanager.services._
import es.tid.cosmos.servicemanager.services.dependencies.ServiceDependencies
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

/** Manager of the Ambari service configuration workflow.
  * It allows creating clusters with specified services using Ambari.
  *
  * @constructor
  * @param clusterManager the cluster manager
  * @param infrastructureProvider the host-machines provider
  * @param persistentHdfsId the id of the persistent hdfs cluster
  * @param exclusiveMasterSizeCutoff the minimum size of a cluster such that
  *                                  the master node stops acting like a slave
  * @param hadoopConfig parameters that fine-tune configuration of Hadoop
  * @param clusterDao the dao that stores cluster states
  */
class AmbariServiceManager(
    val clusterManager: AmbariClusterManager,
    infrastructureProvider: InfrastructureProvider,
    override val persistentHdfsId: ClusterId,
    exclusiveMasterSizeCutoff: Int,
    hadoopConfig: HadoopConfig,
    clusterDao: ClusterDao) extends ServiceManager with Logging {

  private val dynamicProperties = new AmbariDynamicPropertiesFactory(hadoopConfig, () => for {
    description <- describePersistentHdfsCluster()
    master <- description.master
  } yield master.hostname)

  override def clusterIds: Seq[ClusterId] = clusterDao.ids

  override def describeCluster(id: ClusterId): Option[ImmutableClusterDescription] =
    clusterDao.getDescription(id).map(_.view)

  override val optionalServices: Set[Service] = OptionalServices

  override def createCluster(
      name: ClusterName,
      clusterSize: Int,
      serviceInstances: Set[AnyServiceInstance],
      users: Seq[ClusterUser],
      preConditions: ClusterExecutableValidation): NowFuture[ClusterId, Unit] = {
    val servicesWithDependencies = ServiceDependencies.executionPlan(
      missingBasicServices(serviceInstances) ++ serviceInstances + CosmosUserService.instance(users)
    )
    val clusterDescription = clusterDao.registerCluster(
      name = name,
      size = clusterSize,
      services = servicesWithDependencies.map(_.service).toSet
    )
    val creation_> = clusterDescription.withFailsafe {
      for {
        machines <- infrastructureProvider.createMachines(
          preConditions(clusterDescription.id), MachineProfile.G1Compute, clusterSize, waitForSsh)
        (master, slaves) = masterAndSlaves(machines)
        _ = setMachineInfo(clusterDescription, master, slaves)
        _ <- createCluster(clusterDescription, servicesWithDependencies)
        _ = clusterDao.setUsers(clusterDescription.id, users.toSet)
      } yield ()
    }
    (clusterDescription.id, creation_>)
  }

  private def missingBasicServices(serviceInstances: Set[AnyServiceInstance]) =
    (BasicHadoopServices diff serviceInstances.map(_.service)).map(
      service => service.defaultInstance.getOrElse(throw new IllegalArgumentException(
        s"A basic service that needs configuration was not configured: ${service.name}")))

  private def masterAndSlaves(machines: Seq[MachineState]) = machines match {
    case Seq(master, slaves @ _*) if machines.length < exclusiveMasterSizeCutoff =>
      (master, machines)
    case Seq(master, slaves @ _*) => (master, slaves)
    case _ => throw new IllegalArgumentException("Need at least one machine")
  }

  private def createCluster(
      dbClusterDescription: MutableClusterDescription,
      serviceDescriptions: Seq[AnyServiceInstance]) = {
    require(dbClusterDescription.master.isDefined,
      "This function cannot be called if the master has not been set")
    require(dbClusterDescription.slaves.nonEmpty,
      "This function cannot be called if the slaves have not been set")
    val master = dbClusterDescription.master.get
    val deployment_> = for {
      configuration <- clusterManager.deployCluster(
        dbClusterDescription.view, serviceDescriptions, dynamicProperties)
    } yield {
      dbClusterDescription.nameNode = toNameNodeUri(master)
      dbClusterDescription.state = Running
      configuration.services.find(_.configType == "infinity-driver").foreach { serviceConfig =>
        val blockedPortsString = serviceConfig.properties("blocked_ports").asInstanceOf[String]
        dbClusterDescription.blockedPorts = blockedPortsString.split(',').map(_.toInt).toSet
      }
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
        _ <- clusterManager.removeCluster(cluster)
        machines <- infrastructureProvider.assignedMachines(cluster.machines.map(_.hostname))
        _ <- infrastructureProvider.releaseMachines(machines)
      } yield {
        mutableCluster.state = Terminated
      }
    }
  }

  override def listUsers(clusterId: ClusterId): Option[Seq[ClusterUser]] =
    clusterDao.getUsers(clusterId).map(_.toSeq)

  override def setUsers(
      clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] = {
    val mutableCluster = clusterDao.getDescription(clusterId).getOrElse(
      throw new IllegalArgumentException(
        s"Cluster with id [$clusterId] is not managed by this ServiceManager")
    )
    mutableCluster.withFailsafe {
      val cluster = mutableCluster.view
      val clusterState = cluster.state
      require(clusterState == Running, s"Cluster[$clusterId] not Running")
      val delta = clusterUsersDelta(listUsers(clusterId), users)
      for {
        _ <- clusterManager.changeServiceConfiguration(
          cluster,
          dynamicProperties,
          CosmosUserService.instance(delta)
        )
      } yield {
        clusterDao.setUsers(clusterId, users.toSet)
      }
    }
  }

  private def clusterUsersDelta(
      currentUsers: Option[Seq[ClusterUser]],
      newUsers: Seq[ClusterUser]): Seq[ClusterUser] =
    (newUsers.toSet diff currentUsers.toSet.flatten).toSeq

  override def deployPersistentHdfsCluster(parameters: InfinityServerParameters): Future[Unit] = for {
    machineCount <- infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave)
    services = persistentHdfsServices(parameters)
    clusterDescription = clusterDao.registerCluster(
      id = persistentHdfsId,
      name = ClusterName(persistentHdfsId.id),
      size = machineCount + 1,
      services = services.map(_.service).toSet
    )
    description <- clusterDescription.withFailsafe(for {
      master <- infrastructureProvider.createMachines(
        PassThrough, MachineProfile.HdfsMaster, numberOfMachines = 1, waitForSsh).map(_.head)
      slaves <- infrastructureProvider.createMachines(
        PassThrough, MachineProfile.HdfsSlave, numberOfMachines = machineCount, waitForSsh)
      _ = setMachineInfo(clusterDescription, master, slaves)
      _ <- createCluster(clusterDescription, services)
    } yield ())
  } yield description

  override def clusterNodePoolCount: Int =
    infrastructureProvider.machinePoolCount(_ == MachineProfile.G1Compute)
}

private[ambari] object AmbariServiceManager {
  val BasicHadoopServices: Set[Service] =
    Set(MapReduce2, InfinityDriver, Hdfs)

  val OptionalServices: Set[Service] = Set(Hive, Oozie, Pig, Sqoop)

  def persistentHdfsServices(infinityParameters: InfinityServerParameters): Seq[AnyServiceInstance] =
    InfinityServer.instance(infinityParameters) +: Seq(
      Zookeeper.defaultInstance,
      Hdfs.defaultInstance,
      CosmosUserService.defaultInstance
    ).flatten

  private def setMachineInfo(
      description: MutableClusterDescription,
      master: MachineState,
      slaves: Seq[MachineState]): Unit = {
    description.master = toHostInfo(master)
    description.slaves = slaves.map(toHostInfo)
  }

  private def toHostInfo(host: MachineState) = HostDetails(host.hostname, host.ipAddress)

  private def waitForSsh(state: MachineState): Future[Unit] =
    TcpServerWatcher(state.hostname, SshServicePort).waitForServer()
}
