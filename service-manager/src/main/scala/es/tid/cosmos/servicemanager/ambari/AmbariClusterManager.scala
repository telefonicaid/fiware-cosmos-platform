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

import es.tid.cosmos.servicemanager.{ComponentDescription, ClusterManager}
import es.tid.cosmos.servicemanager.ambari.configuration.FileConfigurationContributor
import es.tid.cosmos.servicemanager.ambari.rest.{Host, ServiceClient, AmbariServer, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.{AmbariServiceFactory, AmbariServiceDetails, AmbariService}
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ImmutableClusterDescription, ClusterId}

private[ambari] class AmbariClusterManager(
    ambariServer: AmbariServer,
    rootPrivateSshKey: String,
    override val configPath: String) extends ClusterManager with FileConfigurationContributor {
  import AmbariClusterManager._

  override type ServiceDescriptionType = AmbariService

  override val configName = "global-basic"

  override def deployCluster(
      clusterDescription: ImmutableClusterDescription,
      serviceDescriptions: Seq[ServiceDescriptionType],
      dynamicProperties: DynamicPropertiesFactory): Future[Unit] = for {
    cluster <- initCluster(clusterDescription)
    master = clusterDescription.master.get
    slaves = clusterDescription.slaves
    hosts <- cluster.addHosts(clusterDescription.machines.map(_.hostname))
    masterHost = hosts.find(_.name == master.hostname).get
    slaveHosts = hosts.filter(host => slaves.exists(_.hostname == host.name))
    _ <- Configurator.applyConfiguration(
      cluster,
      properties = dynamicProperties.forCluster(masterHost.name, slaveHosts.map(_.name)),
      contributors = this +: serviceDescriptions)
    services <- Future.traverse(serviceDescriptions)(
      srv => createService(srv.details, cluster, masterHost, slaveHosts))
    deployedServices <- installInOrder(services)
  } yield ()

  /** Create a service instance on a given cluster.
    *
    * @param ambariService  service to create
    * @param cluster the cluster for which to instantiate the service
    * @param master the cluster's master node
    * @param slaves the cluster's slave nodes
    * @return the future of the service instance for the given cluster
    */
  private def createService(
      ambariService: AmbariServiceDetails,
      cluster: Cluster,
      master: Host,
      slaves: Seq[Host]): Future[ServiceClient] = {

    def addComponents(hosts: Seq[Host], componentFilter: ComponentDescription => Boolean) =
      Future.sequence(
        hosts.map(_.addComponents(ambariService.components.filter(componentFilter).map(_.name))))

    for {
      service <- cluster.addService(ambariService.service.name)
      _ <- Future.sequence(ambariService.components.map(component => service.addComponent(component.name)))
      _ <- master.addComponents(ambariService.components.filter(_.isMaster).map(_.name))
      (masterAndSlaveHost, exclusiveSlaves) = slaves.partition(_.name == master.name)
      _ <- addComponents(exclusiveSlaves, _.isSlave)
      _ <- addComponents(masterAndSlaveHost, c => c.isSlave && !c.isMaster)
    } yield service
  }

  override def removeCluster(cluster: ClusterDescription): Future[Unit] = for {
    _ <- stopStartedServices(cluster.id)
    _ <- ambariServer.removeCluster(cluster.id.toString)
  } yield ()

  override def changeServiceConfiguration(
      clusterDescription: ImmutableClusterDescription,
      dynamicProperties: DynamicPropertiesFactory,
      serviceDescription: AmbariService): Future[Any] = for {
    cluster <- ambariServer.getCluster(clusterDescription.id.toString)
    service <- cluster.getService(serviceDescription.name)
    master <- ServiceMasterExtractor.getServiceMaster(cluster, serviceDescription.details)
    stoppedService <- service.stop()
    slaveDetails = clusterDescription.slaves
    slaves <- Future.traverse(slaveDetails)(details => cluster.getHost(details.hostname))
    properties = dynamicProperties.forCluster(master.name, slaves.map(_.name))
    _ <- Configurator.applyConfiguration(cluster, properties, List(serviceDescription))
    startedService <- stoppedService.start()
  } yield startedService

  private def initCluster(description: ImmutableClusterDescription) : Future[Cluster] = {
    val distinctHostnames = description.machines.map(_.hostname).toSet
    for {
      _ <- ambariServer.bootstrapMachines(distinctHostnames, rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- ambariServer.createCluster(description.id.toString, StackVersion)
    } yield cluster
  }

  private def installInOrder(services: Seq[ServiceClient]): Future[Seq[ServiceClient]] = {
    def doInstall(
        installedServices_> : Future[Seq[ServiceClient]],
        service : ServiceClient): Future[Seq[ServiceClient]] = for {
      installedServices <- installedServices_>
      service <- installAndStart(service)
    } yield service +: installedServices
    services.foldLeft(Future.successful(Seq[ServiceClient]()))(doInstall)
  }

  private def installAndStart(service : ServiceClient): Future[ServiceClient] = for {
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

private[ambari] object AmbariClusterManager {
  val StackVersion = "HDP-2.0.6_Cosmos"
}
