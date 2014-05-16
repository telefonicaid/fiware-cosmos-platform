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

import scala.concurrent.{blocking, Future}
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration.{Configurator, ConfigurationContributor, FileConfigurationContributor}
import es.tid.cosmos.servicemanager.ambari.rest.{Host, ServiceClient, AmbariServer, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.{ComponentDescription, AmbariService}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.configuration.{ConfigurationBundle, DynamicPropertiesFactory}
import es.tid.cosmos.servicemanager.services.Service

private[ambari] class AmbariClusterManager(
    ambariServer: AmbariServer,
    rootPrivateSshKey: String,
    configPath: String,
    serviceLookup: Service => AmbariService) extends ClusterManager {

  val globalConfiguration = new FileConfigurationContributor(configPath, "global-basic")

  override def deployCluster(
      clusterDescription: ImmutableClusterDescription,
      serviceInstances: Seq[AnyServiceInstance],
      dynamicProperties: DynamicPropertiesFactory): Future[ConfigurationBundle] = {
    val services: Seq[AmbariService] = serviceInstances.map(_.service).map(serviceLookup)
    for {
      cluster <- initCluster(clusterDescription)
      master = clusterDescription.master.get
      slaves = clusterDescription.slaves
      hosts <- cluster.addHosts(clusterDescription.machines.map(_.hostname))
      masterHost = hosts.find(_.name == master.hostname).get
      slaveHosts = hosts.filter(host => slaves.exists(_.hostname == host.name))
      configuration <- Configurator.applyConfiguration(
        cluster,
        properties = dynamicProperties.forCluster(masterHost.name, slaveHosts.map(_.name)),
        contributors = configuratorContributors(serviceInstances)
      )
      serviceClients <- Future.traverse(services) { srv =>
        createService(srv, cluster, masterHost, slaveHosts)
      }
      deployedServices <- installInOrder(serviceClients)
    } yield configuration
  }

  private def configuratorContributors(
      serviceInstances: Seq[AnyServiceInstance]): Seq[ConfigurationContributor] =
    serviceInstances.map(configuratorContributor) :+ globalConfiguration

  private def configuratorContributor(serviceInstance: AnyServiceInstance): ConfigurationContributor = {
    val ambariService = serviceLookup(serviceInstance.service)
    ambariService.configurator(
      serviceInstance.parameter.asInstanceOf[ambariService.service.Parametrization],
      configPath
    )
  }

  /** Create a service instance on a given cluster.
    *
    * @param ambariService  service to create
    * @param cluster the cluster for which to instantiate the service
    * @param master the cluster's master node
    * @param slaves the cluster's slave nodes
    * @return the future of the service instance for the given cluster
    */
  private def createService(
      ambariService: AmbariService,
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
      serviceInstance: AnyServiceInstance): Future[Any] = {
    val ambariService = serviceLookup(serviceInstance.service)
    for {
      cluster <- ambariServer.getCluster(clusterDescription.id.toString)
      service <- cluster.getService(serviceInstance.service.name)
      master <- ServiceMasterExtractor.getServiceMaster(cluster, ambariService)
      stoppedService <- service.stop()
      slaveDetails = clusterDescription.slaves
      slaves <- Future.traverse(slaveDetails)(details => cluster.getHost(details.hostname))
      properties = dynamicProperties.forCluster(master.name, slaves.map(_.name))
      _ <- Configurator.applyConfiguration(
        cluster, properties, Seq(configuratorContributor(serviceInstance)))
      startedService <- stoppedService.start()
    } yield startedService
  }

  private def initCluster(description: ImmutableClusterDescription) : Future[Cluster] = {
    val distinctHostnames = description.machines.map(_.hostname).toSet
    for {
      _ <- ambariServer.bootstrapMachines(distinctHostnames, rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- ambariServer.createCluster(
        description.id.toString, AmbariClusterManager.StackVersion)
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
