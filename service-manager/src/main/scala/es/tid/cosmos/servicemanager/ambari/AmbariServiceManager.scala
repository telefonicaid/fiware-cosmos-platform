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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.{Await, Future}
import scala.concurrent.duration.{FiniteDuration, Duration}

import com.typesafe.scalalogging.slf4j.Logging

import es.tid.cosmos.platform.ial.{MachineState, MachineProfile, InfrastructureProvider}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.machines._
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, ClusterProvisioner, Cluster}
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.util.TcpServer
import es.tid.cosmos.servicemanager.util.TcpServer.SshService

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
 * @param mappersPerSlave the number of map slots per tasktracker
 * @param reducersPerSlave the number of reducer slots per tasktracker
 */
class AmbariServiceManager(
    override protected val provisioner: ClusterProvisioner,
    override protected val infrastructureProvider: InfrastructureProvider,
    override protected val refreshGracePeriod: FiniteDuration,
    override val persistentHdfsId: ClusterId,
    mappersPerSlave: Int,
    reducersPerSlave: Int)
  extends ServiceManager with FileConfigurationContributor
  with Refreshing with Logging {

  override type ServiceDescriptionType = AmbariServiceDescription

  private val stackVersion = "Cosmos-0.1.0"

  @volatile var clusters = Map[ClusterId, MutableClusterDescription]()

  // perform a sync with Ambari upon initialization
  refresh()

  override def clusterIds: Seq[ClusterId] = clusters.keys.toSeq

  override def describeCluster(id: ClusterId): Option[ClusterDescription] =
    clusters.get(id).map(_.view)

  override def services(user: ClusterUser): Seq[ServiceDescriptionType] =
    Seq(Hdfs, MapReduce, Oozie, new CosmosUserService(Seq(user)))

  /**
   * Wait until all pending operations are finished
   */
  def close() {
    clusters.values.foreach(description => Await.ready(description.deployment_>, Duration.Inf))
  }

  override def createCluster(
      name: String,
      clusterSize: Int,
      serviceDescriptions: Seq[ServiceDescriptionType]): ClusterId = {
    val id = new ClusterId
    val machines_> =
      infrastructureProvider.createMachines(name, MachineProfile.G1Compute, clusterSize, waitForSsh)
    val deployment_> = for {
      machines <- machines_>
      (master, slaves) = masterAndSlaves(machines)
      deployment <- createUnregisteredCluster(id, name, serviceDescriptions, master, slaves)
    } yield deployment
    val nameNode_> = mapMaster(machines_>, toNameNodeUri)
    registerCluster(new MutableClusterDescription(
      id, name, clusterSize, deployment_>, machines_>, nameNode_>))
    id
  }

  private def createUnregisteredCluster(
      id: ClusterId,
      name: String,
      serviceDescriptions: Seq[ServiceDescriptionType],
      master : MachineState,
      slaves: Seq[MachineState]) = {
    for {
      cluster <- initCluster(id, master +: slaves)
      masterHost <- cluster.addHost(master.hostname)
      nonMasterHosts <- cluster.addHosts(slaves
        .filterNot(_.hostname == master.hostname)
        .map(_.hostname))
      slaveHosts = if (nonMasterHosts.length < slaves.length) masterHost +: nonMasterHosts
        else nonMasterHosts
      configuredCluster <- applyConfiguration(
        cluster,
        masterHost,
        slaveHosts,
        this::serviceDescriptions.toList)
      services <- Future.traverse(serviceDescriptions)(
        srv => srv.createService(configuredCluster, masterHost, slaveHosts))
      deployedServices <- installInOrder(services)
    } yield deployedServices
  }

  override protected def registerCluster(description: MutableClusterDescription) {
    clusters.synchronized {
      clusters = clusters.updated(description.id, description)
      logger.info(s"Cluster [${description.id}] registered")
    }
  }

  private def waitForSsh(state: MachineState): Future[Unit] =
    TcpServer(state.hostname, SshService).waitForServer()

  private def initCluster(id: ClusterId, machines: Seq[MachineState]) : Future[Cluster] = {
    val distinctHostnames = machines.map(_.hostname).distinct
    for {
      _ <- provisioner.bootstrapMachines(
        distinctHostnames,
        infrastructureProvider.rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- provisioner.createCluster(id.toString, stackVersion)
    } yield cluster
  }

  private def ensureHostsRegistered(hostnames: Seq[String]): Future[Unit] =
    provisioner.registeredHostnames.flatMap(registeredHostnames =>
      if (hostnames.diff(registeredHostnames).nonEmpty) {
        Thread.sleep(1000)
        ensureHostsRegistered(hostnames)
      } else
        Future.successful())

  private def stopService(cluster: Cluster)(name: String): Future[Service] =
    for {
      service <- cluster.getService(name)
      stoppedService <- service.stop()
    } yield stoppedService

  override def terminateCluster(id: ClusterId): Future[Unit] = {
    require(clusters.contains(id), s"Cluster $id does not exist")
    val stoppedServices_> = for {
      cluster <- provisioner.getCluster(id.toString)
      stoppedServices <- Future.traverse(cluster.serviceNames)(stopService(cluster))
    } yield stoppedServices

    val termination_> = for {
      _ <- stoppedServices_>
      _ <- provisioner.removeCluster(id.toString)
      machines <- clusters(id).machines_>
      distinctHostnames = machines.map(_.hostname).distinct
      _ <- provisioner.teardownMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
      _ <- infrastructureProvider.releaseMachines(machines)
    } yield ()
    clusters(id).terminate(termination_>)
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

  private def installInOrder(services: Seq[Service]): Future[List[Service]] = {
    def doInstall(
        installedServices_> : Future[List[Service]], service : Service): Future[List[Service]] = {
      for {
        installedServices <- installedServices_>
        service <- installAndStart(service)
      } yield service :: installedServices
    }
    services.foldLeft(Future.successful(List[Service]()))(doInstall)
  }

  private def installAndStart(service : Service): Future[Service] = {
    for {
      installedService <- service.install()
      startedService <- installedService.start()
    } yield startedService
  }

  private def applyConfiguration(
      cluster: Cluster,
      master: Host,
      slaves: Seq[Host],
      contributors: List[ConfigurationContributor]): Future[Cluster] = {
    val properties = Map(
      ConfigurationKeys.HdfsReplicationFactor -> Math.min(3, slaves.length).toString,
      ConfigurationKeys.MasterNode -> master.name,
      ConfigurationKeys.MappersPerSlave -> mappersPerSlave.toString,
      ConfigurationKeys.MaxMapTasks -> (mappersPerSlave * slaves.length).toString,
      ConfigurationKeys.MaxReduceTasks -> (1.75 * reducersPerSlave * slaves.length).round.toString,
      ConfigurationKeys.ReducersPerSlave -> reducersPerSlave.toString)
    Configurator.applyConfiguration(cluster, properties, contributors).map(_ => cluster)
  }

  private def changeServiceConfiguration(
      id: ClusterId,
      clusterDescription: ClusterDescription,
      serviceDescription: AmbariServiceDescription): Future[Service] = {
    for {
      cluster <- provisioner.getCluster(id.toString)
      service <- cluster.getService(serviceDescription.name)
      master <- ServiceMasterExtractor.getServiceMaster(cluster, serviceDescription)
      stoppedService <- service.stop()
      slaveDetails <- clusterDescription.slaves_>
      slaves <- Future.traverse(slaveDetails)(details => cluster.getHost(details.hostname))
      _ <- applyConfiguration(cluster, master, slaves, List(serviceDescription))
      startedService <- stoppedService.start()
    } yield startedService
  }

  override def deployPersistentHdfsCluster(): Future[Unit] = {
    val masterMachine_> = infrastructureProvider.createMachines(
      persistentHdfsId.id, MachineProfile.HdfsMaster, 1, waitForSsh)
    for {
      machineCount <- infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave)
      slaveMachines <- infrastructureProvider.createMachines(
        persistentHdfsId.id, MachineProfile.HdfsSlave, machineCount, waitForSsh)
      Seq(masterMachine) <- masterMachine_>
      _ <- createUnregisteredCluster(
        id = persistentHdfsId,
        name = persistentHdfsId.id,
        serviceDescriptions = Seq(Hdfs, new CosmosUserService(Seq())),
        master = masterMachine,
        slaves = slaveMachines)
    } yield ()
  }
}
