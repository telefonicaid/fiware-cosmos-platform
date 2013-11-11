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

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.typesafe.scalalogging.slf4j.Logging

import es.tid.cosmos.platform.ial.{MachineState, MachineProfile, InfrastructureProvider}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.machines._
import es.tid.cosmos.servicemanager.ambari.rest.{AmbariServer, Cluster, Service}
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.util.TcpServer
import es.tid.cosmos.servicemanager.util.TcpServer._

/**
 * Manager of the Ambari service configuration workflow.
 * It allows creating clusters with specified services using Ambari.
 *
 * @constructor
 * @param ambariServer the cluster provisioner
 * @param infrastructureProvider the host-machines provider
 * @param persistentHdfsId the id of the persistent hdfs cluster
 * @param clusterDao the dao that stores cluster states
 * @param hadoopConfig parameters that fine-tune configuration of Hadoop
 */
class AmbariServiceManager(
    ambariServer: AmbariServer,
    infrastructureProvider: InfrastructureProvider,
    override val persistentHdfsId: ClusterId,
    hadoopConfig: HadoopConfig,
    clusterDao: AmbariClusterDao)
  extends ServiceManager with FileConfigurationContributor with Logging {
  import AmbariServiceManager._

  override type ServiceDescriptionType = AmbariServiceDescription

  override def clusterIds: Seq[ClusterId] = clusterDao.ids

  override def describeCluster(id: ClusterId): Option[ImmutableClusterDescription] =
    clusterDao.getDescription(id).map(_.view)

  /** Exposed services as optional choices */
  override val services: Seq[ServiceDescriptionType] = AmbariServiceManager.services

  private def userServices(users: Seq[ClusterUser]): Seq[ServiceDescriptionType] =
    Seq(new CosmosUserService(users))

  override def createCluster(
      name: String,
      clusterSize: Int,
      serviceDescriptions: Seq[ServiceDescriptionType],
      users: Seq[ClusterUser]): ClusterId = {
    val id = ClusterId()
    createCluster(
      id,
      name,
      clusterSize,
      masterProfile = MachineProfile.G1Compute,
      slavesProfile = MachineProfile.G1Compute,
      Seq(Hdfs, MapReduce) ++ serviceDescriptions ++ userServices(users)
    )
    id
  }

  private def createCluster(
      id: ClusterId,
      name: String,
      clusterSize: Int,
      masterProfile: MachineProfile.Value,
      slavesProfile: MachineProfile.Value,
      serviceDescriptions: Seq[ServiceDescriptionType]) = {
    val (master_>, slaves_>) = createMachines(masterProfile, slavesProfile, clusterSize)
    val dbClusterDescription = clusterDao.registerCluster(id, name, clusterSize)
    dbClusterDescription.state = Provisioning
    for (master <- master_>) {
      dbClusterDescription.master = toHostInfo(master)
    }
    for (slaves <- slaves_>) {
      dbClusterDescription.slaves = slaves.map(toHostInfo)
    }
    val deployment_> = for {
      master <- master_>
      slaves <- slaves_>
      deployment <- deployCluster(id, name, serviceDescriptions, master, slaves, hadoopConfig)
    } yield {
      dbClusterDescription.nameNode = toNameNodeUri(master)
      dbClusterDescription.state = Running
      deployment
    }
    deployment_>.onFailure {
      case err => dbClusterDescription.state = Failed(err)
    }
    deployment_>
  }

  private def createMachines(
      masterProfile: MachineProfile.Value,
      slavesProfile: MachineProfile.Value,
      numberOfMachines: Int): (Future[MachineState], Future[Seq[MachineState]]) = {
    if (masterProfile == slavesProfile) {
      val x = for {
        machines <- infrastructureProvider.createMachines(masterProfile, numberOfMachines, waitForSsh)
      } yield masterAndSlaves(machines)
      (x.map(_._1), x.map(_._2))
    } else {
      val master_> = infrastructureProvider.createMachines(masterProfile, 1, waitForSsh).map(_.head)
      val slaves_> =
        if (numberOfMachines > 1)
          infrastructureProvider.createMachines(slavesProfile, numberOfMachines - 1, waitForSsh)
        else throw new IllegalArgumentException("Machine creation request with 1 server " +
          "indicating different profiles for master and slaves")
      (master_>, slaves_>)
    }
  }

  private def deployCluster(
      id: ClusterId,
      name: String,
      serviceDescriptions: Seq[AmbariServiceDescription],
      master : MachineState,
      slaves: Seq[MachineState],
      hadoopConfig: HadoopConfig) = for {
    cluster <- initCluster(id, master +: slaves)
    masterHost <- cluster.addHost(master.hostname)
    nonMasterHosts <- cluster.addHosts(slaves
      .filterNot(_.hostname == master.hostname)
      .map(_.hostname))
    slaveHosts = if (nonMasterHosts.length < slaves.length)
      masterHost +: nonMasterHosts else nonMasterHosts
    configuredCluster <- Configurator.applyConfiguration(
      cluster,
      masterHost,
      slaveHosts,
      hadoopConfig,
      this +: serviceDescriptions)
    services <- Future.traverse(serviceDescriptions)(
      srv => srv.createService(configuredCluster, masterHost, slaveHosts))
    deployedServices <- installInOrder(services)
  } yield deployedServices

  private def waitForSsh(state: MachineState): Future[Unit] =
    TcpServer(state.hostname, SshService).waitForServer()

  private def installInOrder(services: Seq[Service]): Future[Seq[Service]] = {
    def doInstall(
        installedServices_> : Future[Seq[Service]],
        service : Service): Future[Seq[Service]] = for {
      installedServices <- installedServices_>
      service <- installAndStart(service)
    } yield service +: installedServices
    services.foldLeft(Future.successful(Seq[Service]()))(doInstall)
  }

  private def installAndStart(service : Service): Future[Service] = for {
    installedService <- service.install()
    startedService <- installedService.start()
  } yield startedService

  private def initCluster(id: ClusterId, machines: Seq[MachineState]) : Future[Cluster] = {
    val distinctHostnames = machines.map(_.hostname).toSet
    for {
      _ <- ambariServer.bootstrapMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- ambariServer.createCluster(id.toString, StackVersion)
    } yield cluster
  }

  private def ensureHostsRegistered(hostnames: Set[String]): Future[Unit] =
    ambariServer.registeredHostnames.flatMap(registeredHostnames =>
      if (hostnames.diff(registeredHostnames).nonEmpty) {
        Thread.sleep(1000)
        ensureHostsRegistered(hostnames)
      } else
        Future.successful())

  override def terminateCluster(id: ClusterId): Future[Unit] = {
    val mutableCluster = clusterDao.getDescription(id).getOrElse(
      throw new IllegalArgumentException(s"Cluster $id does not exist"))
    val cluster = mutableCluster.view
    val clusterState = cluster.state
    require(!Seq(Provisioning, Terminating, Terminated).contains(clusterState),
      s"Cluster $id is in state $clusterState, which is not a valid state for termination")
    mutableCluster.state = Terminating
    val termination_> = for {
      _ <- removeClusterFromAmbari(cluster)
      machines <- infrastructureProvider.assignedMachines(cluster.machines.map(_.hostname))
      _ <- infrastructureProvider.releaseMachines(machines)
    } yield {
      mutableCluster.state = Terminated
      ()
    }
    termination_>.onFailure {
      case err => mutableCluster.state = Failed(err)
    }
    termination_>
  }

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
        new CosmosUserService(users))
    } yield ()
  }

  override val configName = "global-basic"

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
      cluster, master, slaves, hadoopConfig, List(serviceDescription))
    startedService <- stoppedService.start()
  } yield startedService

  private def removeClusterFromAmbari(cluster: ClusterDescription) = for {
    _ <- stopStartedServices(cluster.id)
    _ <- ambariServer.removeCluster(cluster.id.toString)
    distinctHostnames = cluster.machines.map(_.hostname).toSet
    _ <- ambariServer.teardownMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
  } yield ()

  private def stopStartedServices(id: ClusterId) = for {
    cluster <- ambariServer.getCluster(id.toString)
    services <- Future.traverse(cluster.serviceNames)(cluster.getService)
    startedServices = services.filter(_.state == "STARTED")
    stoppedServices <- Future.traverse(startedServices)(_.stop())
  } yield stoppedServices

  override def deployPersistentHdfsCluster(): Future[Unit] = for {
    machineCount <- infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave)
    deployment_> = createCluster(
        id = persistentHdfsId,
        name = persistentHdfsId.id,
        clusterSize = machineCount + 1,
        masterProfile = MachineProfile.HdfsMaster,
        slavesProfile = MachineProfile.HdfsSlave,
        serviceDescriptions = Seq(Hdfs, new CosmosUserService(Seq())))
    _ <- deployment_>
  } yield ()

  override def describePersistentHdfsCluster(): Option[ImmutableClusterDescription] =
    describeCluster(persistentHdfsId)

  override def terminatePersistentHdfsCluster(): Future[Unit] = terminateCluster(persistentHdfsId)
}

private[ambari] object AmbariServiceManager {
  val StackVersion = "Cosmos-0.1.0"
  /** Exposed services as optional choices */
  val services: Seq[AmbariServiceDescription] = Seq(Hive, Oozie, Pig, Sqoop)
  val allServices = Seq(Hdfs, MapReduce, CosmosUserService) ++ services
}
