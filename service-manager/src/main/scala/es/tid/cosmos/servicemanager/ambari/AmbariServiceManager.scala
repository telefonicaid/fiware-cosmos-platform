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
import scala.concurrent.duration._

import akka.actor.{Props, ActorSystem, ActorRef}
import akka.pattern.ask
import akka.util.Timeout
import com.typesafe.scalalogging.slf4j.Logging

import es.tid.cosmos.platform.ial.{MachineProfile, InfrastructureProvider}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.rest.{Service, ClusterProvisioner}
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.ambari.ClusterFactory._

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
    initializationPeriod: FiniteDuration,
    override protected val refreshGracePeriod: FiniteDuration,
    override val persistentHdfsId: ClusterId,
    mappersPerSlave: Int,
    reducersPerSlave: Int)
  extends ServiceManager with FileConfigurationContributor
  with Refreshing with Logging {

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

  private val actorSystem = ActorSystem("AmbariServiceManagerActorSystem")
  implicit private val requestTimeout = Timeout(10 seconds)
  private val clusterFactory: ActorRef = actorSystem.actorOf(
    Props(classOf[ClusterFactory], infrastructureProvider, provisioner))

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
      users: Seq[ClusterUser]): ClusterId =
    Await.result(createCluster(
      new ClusterId,
      name,
      clusterSize,
      masterProfile = MachineProfile.G1Compute,
      slavesProfile = MachineProfile.G1Compute,
      Seq(Hdfs, MapReduce) ++ serviceDescriptions ++ userServices(users)
    ), requestTimeout.duration).id

  private def createCluster(
      id: ClusterId,
      name: String,
      clusterSize: Int,
      masterProfile: MachineProfile.Value,
      slavesProfile: MachineProfile.Value,
      serviceDescriptions: Seq[ServiceDescriptionType]) = {
    val createClusterMsg = CreateClusterMsg(
      id, name, clusterSize, masterProfile, slavesProfile,
      ClusterFactory.HadoopConfig(mappersPerSlave, reducersPerSlave), serviceDescriptions)
    for { reply <- (clusterFactory ? createClusterMsg).mapTo[ClusterCreatedMsg] }
    yield {
      registerCluster(reply.cluster)
      reply.cluster
    }
  }

  override protected def registerCluster(description: MutableClusterDescription) {
    clusters.synchronized {
      clusters = clusters.updated(description.id, description)
      logger.info(s"Cluster [${description.id}] registered")
    }
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
    for {
      _ <- (clusterFactory ? TerminateClusterMsg(clusters(id))).mapTo[ClusterTerminatedMsg]
    } yield ()
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
      serviceDescription: AmbariServiceDescription): Future[Service] = {
    for {
      cluster <- provisioner.getCluster(id.toString)
      service <- cluster.getService(serviceDescription.name)
      master <- ServiceMasterExtractor.getServiceMaster(cluster, serviceDescription)
      stoppedService <- service.stop()
      slaveDetails <- clusterDescription.slaves_>
      slaves <- Future.traverse(slaveDetails)(details => cluster.getHost(details.hostname))
      _ <- Configurator.applyConfiguration(
        cluster, master, slaves, mappersPerSlave, reducersPerSlave, List(serviceDescription))
      startedService <- stoppedService.start()
    } yield startedService
  }

  override def deployPersistentHdfsCluster(): Future[Unit] = {
    for {
      machineCount <- infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave)
      cluster <- createCluster(
          id = persistentHdfsId,
          name = persistentHdfsId.id,
          clusterSize = machineCount + 1,
          masterProfile = MachineProfile.HdfsMaster,
          slavesProfile = MachineProfile.HdfsSlave,
          serviceDescriptions = Seq(Hdfs, new CosmosUserService(Seq())))
      _ <- cluster.deployment_>
    } yield ()
  }

  override def describePersistentHdfsCluster(): Option[ClusterDescription] =
    describeCluster(persistentHdfsId)

  override def terminatePersistentHdfsCluster(): Future[Unit] = terminateCluster(persistentHdfsId)
}
