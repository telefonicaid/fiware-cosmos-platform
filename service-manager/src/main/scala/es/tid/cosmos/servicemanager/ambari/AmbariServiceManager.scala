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

import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

import com.typesafe.scalalogging.slf4j.Logging

import es.tid.cosmos.platform.ial.{MachineState, MachineProfile, InfrastructureProvider}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.rest.{Cluster, Service, ClusterProvisioner}
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.ambari.machines._
import es.tid.cosmos.servicemanager.util.TcpServer
import es.tid.cosmos.servicemanager.util.TcpServer._

/**
 * Manager of the Ambari service configuration workflow.
 * It allows creating clusters with specified services using Ambari.
 *
 * @constructor
 * @param provisioner the cluster provisioner
 * @param infrastructureProvider the host-machines provider
 * @param persistentHdfsId the id of the persistent hdfs cluster
 * @param refreshGracePeriod the grace period in milliseconds
 *                           to allow for a discovered cluster to stabilize
 * @param hadoopConfig parameters that fine-tune configuration of Hadoop
 */
class AmbariServiceManager(
    override protected val provisioner: ClusterProvisioner,
    override protected val infrastructureProvider: InfrastructureProvider,
    initializationPeriod: FiniteDuration,
    override protected val refreshGracePeriod: FiniteDuration,
    override val persistentHdfsId: ClusterId,
    hadoopConfig: HadoopConfig)
  extends ServiceManager with FileConfigurationContributor
  with Refreshing with Logging {
  import AmbariServiceManager._

  override type ServiceDescriptionType = AmbariServiceDescription

  @volatile var clusters = Map[ClusterId, MutableClusterDescription]()

  override def clusterIds: Seq[ClusterId] = clusters.keys.toSeq

  override def describeCluster(id: ClusterId): Option[ClusterDescription] =
    clusters.get(id).map(_.view)

  /** Exposed services as optional choices */
  override val services: Seq[ServiceDescriptionType] = Seq(Hive, Oozie, Pig, Sqoop)

  override protected val allServices: Seq[AmbariServiceDescription] =
    services ++ Seq(MapReduce, Hdfs, CosmosUserService)

  logger.info("Initialization sync with Ambari")
  Await.result(refresh(), initializationPeriod)

  private def userServices(users: Seq[ClusterUser]): Seq[ServiceDescriptionType] =
    Seq(new CosmosUserService(users))

  /**
   * Wait until all pending operations are finished
   */
  def close() {
    clusters.values.foreach(description => Await.ready(description.deployment_>, Duration.Inf))
  }

  override def createCluster(
      name: String,
      clusterSize: Int,
      serviceDescriptions: Seq[ServiceDescriptionType],
      users: Seq[ClusterUser]): ClusterId = {
    val description = createCluster(
      new ClusterId,
      name,
      clusterSize,
      masterProfile = MachineProfile.G1Compute,
      slavesProfile = MachineProfile.G1Compute,
      Seq(Hdfs, MapReduce) ++ serviceDescriptions ++ userServices(users)
    )
    registerCluster(description)
    description.id
  }

  private def createCluster(
      id: ClusterId,
      name: String,
      clusterSize: Int,
      masterProfile: MachineProfile.Value,
      slavesProfile: MachineProfile.Value,
      serviceDescriptions: Seq[ServiceDescriptionType]) = {
    val (master_>, slaves_>) = createMachines(masterProfile, slavesProfile, clusterSize)
    val machines_> = for {
      master <- master_>
      slaves <- slaves_>
    } yield if (master == slaves.head) slaves else master +: slaves
    val deployment_> = for {
      master <- master_>
      slaves <- slaves_>
      deployment <- deployCluster(
        id, name, serviceDescriptions, master, slaves, hadoopConfig)
    } yield deployment
    val nameNode_> = master_>.map(toNameNodeUri)
    new MutableClusterDescription(
      id, name, clusterSize, deployment_>, machines_>, nameNode_>)
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

  override protected def registerCluster(description: MutableClusterDescription) {
    clusters.synchronized {
      clusters = clusters.updated(description.id, description)
      logger.info(s"Cluster [${description.id}] registered")
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
    val distinctHostnames = machines.map(_.hostname).distinct
    for {
      _ <- provisioner.bootstrapMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- provisioner.createCluster(id.toString, StackVersion)
    } yield cluster
  }

  private def ensureHostsRegistered(hostnames: Seq[String]): Future[Unit] =
    provisioner.registeredHostnames.flatMap(registeredHostnames =>
      if (hostnames.diff(registeredHostnames).nonEmpty) {
        Thread.sleep(1000)
        ensureHostsRegistered(hostnames)
      } else
        Future.successful())

  override def terminateCluster(id: ClusterId): Future[Unit] = {
    val maybeCluster = clusters.get(id)
    require(maybeCluster.isDefined, s"Cluster $id does not exist")
    val cluster = maybeCluster.get
    val clusterState = cluster.state
    require(!Seq(Provisioning, Terminating, Terminated).contains(clusterState),
      s"Cluster $id is in state $clusterState, which is not a valid state for termination")
    val termination_> = for {
      _ <- removeClusterFromAmbari(cluster)
      machines <- cluster.machines_>
      _ <- infrastructureProvider.releaseMachines(machines)
    } yield ()
    cluster.signalTermination(termination_>)
    termination_>
  }

  override def addUsers(clusterId: ClusterId, users: ClusterUser*): Future[Unit] = {
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
    cluster <- provisioner.getCluster(id.toString)
    service <- cluster.getService(serviceDescription.name)
    master <- ServiceMasterExtractor.getServiceMaster(cluster, serviceDescription)
    stoppedService <- service.stop()
    slaveDetails <- clusterDescription.slaves_>
    slaves <- Future.traverse(slaveDetails)(details => cluster.getHost(details.hostname))
    _ <- Configurator.applyConfiguration(
      cluster, master, slaves, hadoopConfig, List(serviceDescription))
    startedService <- stoppedService.start()
  } yield startedService

  private def removeClusterFromAmbari(cluster: MutableClusterDescription) = for {
    _ <- stopStartedServices(cluster.id)
    _ <- provisioner.removeCluster(cluster.id.toString)
    machines <- cluster.machines_>
    distinctHostnames = machines.map(_.hostname).distinct
    _ <- provisioner.teardownMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
  } yield ()

  private def stopStartedServices(id: ClusterId) = for {
    cluster <- provisioner.getCluster(id.toString)
    services <- Future.traverse(cluster.serviceNames)(cluster.getService)
    startedServices = services.filter(_.state == "STARTED")
    stoppedServices <- Future.traverse(startedServices)(_.stop())
  } yield stoppedServices

  override def deployPersistentHdfsCluster(): Future[Unit] = for {
    machineCount <- infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave)
    cluster = createCluster(
        id = persistentHdfsId,
        name = persistentHdfsId.id,
        clusterSize = machineCount + 1,
        masterProfile = MachineProfile.HdfsMaster,
        slavesProfile = MachineProfile.HdfsSlave,
        serviceDescriptions = Seq(Hdfs, new CosmosUserService(Seq())))
    _ <- cluster.deployment_>
  } yield ()

  override def describePersistentHdfsCluster(): Option[ClusterDescription] =
    describeCluster(persistentHdfsId)

  override def terminatePersistentHdfsCluster(): Future[Unit] = terminateCluster(persistentHdfsId)
}

object AmbariServiceManager {
  val StackVersion = "Cosmos-0.1.0"
}
