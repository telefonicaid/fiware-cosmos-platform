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
import scala.concurrent.duration.Duration

import es.tid.cosmos.platform.ial.{MachineState, MachineProfile, InfrastructureProvider}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
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
 */
class AmbariServiceManager(
    provisioner: ClusterProvisioner,
    infrastructureProvider: InfrastructureProvider)
  extends ServiceManager with ConfigurationContributor with ConfigurationLoader {
  override type ServiceDescriptionType = AmbariServiceDescription

  private val stackVersion = "Cosmos-0.1.0"

  @volatile var clusters = Map[ClusterId, MutableClusterDescription]()

  override def clusterIds: Seq[ClusterId] = clusters.keys.toSeq

  override def describeCluster(id: ClusterId): Option[ClusterDescription] =
    clusters.get(id).map(_.view)

  override def services(user: ClusterUser): Seq[ServiceDescriptionType] =
    Seq(Hdfs, MapReduce, Oozie, new CosmosUserService(user))

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
      infrastructureProvider.createMachines(name, MachineProfile.M, clusterSize, waitForSsh)
    val deployedServices_> = for {
      machines <- machines_>
      cluster <- initCluster(id, machines)
      hosts <- addHosts(machines, cluster)
      (master, slaves) = masterAndSlaves(hosts)
      configuredCluster <- applyConfiguration(cluster, master, this::serviceDescriptions.toList)
      services <- Future.traverse(serviceDescriptions)(
        srv => srv.createService(configuredCluster, master, slaves))
      deployedServices <- installInOrder(services)
    } yield deployedServices
    val description = new MutableClusterDescription(
      id, name, clusterSize, deployedServices_>, machines_>)
    clusters.synchronized {
      clusters = clusters.updated(id, description)
    }
    id
  }

  private def waitForSsh(state: MachineState): Future[Unit] =
    TcpServer(state.hostname, SshService).waitForServer()

  private def initCluster(id: ClusterId, machines: Seq[MachineState]) : Future[Cluster] =
    for {
      _ <- provisioner.bootstrapMachines(machines, infrastructureProvider.rootPrivateSshKey)
      _ <- ensureHostsRegistered(machines.map(_.hostname))
      cluster <- provisioner.createCluster(id.toString, stackVersion)
    } yield cluster

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
      _ <- provisioner.teardownMachines(machines, infrastructureProvider.rootPrivateSshKey)
      _ <- infrastructureProvider.releaseMachines(machines)
    } yield ()
    clusters(id).terminate(termination_>)
    termination_>
  }

  override def contributions(masterName: String) = load("global-basic").build(masterName)

  private def masterAndSlaves(hosts: Seq[Host]) = hosts match {
    case Seq(master) => (master, hosts)
    case Seq(master, slaves @ _*) => (master, slaves)
    case _ => throw new IllegalArgumentException("Need at least one host")
  }

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
      contributors: List[ConfigurationContributor]): Future[Cluster] =
    Configurator.applyConfiguration(cluster, master, contributors).map(_ => cluster)

  private def addHosts(machines : Seq[MachineState], cluster : Cluster): Future[Seq[Host]] =
    Future.traverse(machines)(m => cluster.addHost(m.hostname))
}
