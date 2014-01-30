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

import scala.concurrent.{blocking, Future}
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.servicemanager.ambari.configuration.{FileConfigurationContributor, HadoopConfig}
import es.tid.cosmos.servicemanager.ambari.rest.{Service, AmbariServer, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.clusters.{ImmutableClusterDescription, ClusterDescription, ClusterId}

private[ambari] class ClusterManager(
    ambariServer: AmbariServer,
    rootPrivateSshKey: String) extends FileConfigurationContributor {
  import ClusterManager._

  override val configName = "global-basic"

  def deployCluster(
      clusterDescription: ImmutableClusterDescription,
      serviceDescriptions: Seq[AmbariServiceDescription],
      hadoopConfig: HadoopConfig): Future[Unit] = for {
    cluster <- initCluster(clusterDescription)
    master = clusterDescription.master.get
    slaves = clusterDescription.slaves
    hosts <- cluster.addHosts(clusterDescription.machines.map(_.hostname))
    masterHost = hosts.find(_.name == master.hostname).get
    slaveHosts = hosts.filter(host => slaves.exists(_.hostname == host.name))
    properties = DynamicProperties(hadoopConfig, masterHost.name, slaveHosts.map(_.name))
    _ <- Configurator.applyConfiguration(
      cluster,
      properties,
      contributors = this +: serviceDescriptions)
    services <- Future.traverse(serviceDescriptions)(
      srv => srv.createService(cluster, masterHost, slaveHosts))
    deployedServices <- installInOrder(services)
  } yield ()

  def removeClusterFromAmbari(cluster: ClusterDescription): Future[Unit] = for {
    _ <- stopStartedServices(cluster.id)
    _ <- ambariServer.removeCluster(cluster.id.toString)
  } yield ()

  private def initCluster(description: ImmutableClusterDescription) : Future[Cluster] = {
    val distinctHostnames = description.machines.map(_.hostname).toSet
    for {
      _ <- ambariServer.bootstrapMachines(distinctHostnames, rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- ambariServer.createCluster(description.id.toString, StackVersion)
    } yield cluster
  }

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

  private def ensureHostsRegistered(hostnames: Set[String]): Future[Unit] =
    ambariServer.registeredHostnames.flatMap(registeredHostnames =>
      if (hostnames.diff(registeredHostnames).nonEmpty) {
        blocking { Thread.sleep(1000) }
        ensureHostsRegistered(hostnames)
      } else
        Future.successful())

  private def stopStartedServices(id: ClusterId) = for {
    cluster <- ambariServer.getCluster(id.toString)
    services <- Future.traverse(cluster.serviceNames)(cluster.getService)
    startedServices = services.filter(_.state == "STARTED")
    stoppedServices <- Future.traverse(startedServices)(_.stop())
  } yield stoppedServices
}

private[ambari] object ClusterManager {
  val StackVersion = "HDP-2.0.6_Cosmos"
}
