
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

import es.tid.cosmos.platform.manager.ial.{InfrastructureProvider, MachineProfile, MachineState}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, ClusterProvisioner, Cluster}
import es.tid.cosmos.servicemanager.ambari.services._

/**
 * Manager of the Ambari service configuration workflow.
 * It allows creating clusters with specified services using Ambari.
 *
 * @param provisioner the cluster provisioner
 * @param infrastructureProvider the host-machines provider
 */
class AmbariServiceManager(
    provisioner: ClusterProvisioner,
    infrastructureProvider: InfrastructureProvider)
  extends ServiceManager with ConfigurationContributor with ConfigurationLoader {
  override type ServiceDescriptionType = AmbariServiceDescription

  @volatile var clusters = Map[ClusterId, MutableClusterDescription]()

  override def clusterIds: Seq[ClusterId] = clusters.keys.toSeq

  override def describeCluster(id: ClusterId): Option[ClusterDescription] =
    clusters.get(id).map(_.view)

  override val services: Seq[ServiceDescriptionType] =
    Seq(Hdfs, MapReduce)

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
    val machineFutures = infrastructureProvider.createMachines(
      name, MachineProfile.M, clusterSize).get.toList
    val cluster_> = provisioner.createCluster(name, version = "HDP-1.2.0")
    val (master_>, slaveFutures) = masterAndSlaves(addHosts(machineFutures, cluster_>))
    val configuredCluster_> = applyConfiguration(
      cluster_>, master_>, this::serviceDescriptions.toList)
    val serviceFutures  = serviceDescriptions.map(
      createService(configuredCluster_>, master_>, slaveFutures, _)).toList
    val deployedServices_> = installInOrder(serviceFutures)

    val id = new ClusterId
    val description = new MutableClusterDescription(id, name, clusterSize, deployedServices_>)
    clusters.synchronized {
      clusters = clusters.updated(id, description)
    }
    id
  }

  override def terminateCluster(id: ClusterId): Future[Unit] = {
    throw new UnsupportedOperationException("terminate cluster operation not implemented")
  }

  override def contributions(masterName: String) = load("global-basic").build(masterName)

  private def createService(
    cluster_> : Future[Cluster],
    master_> : Future[Host],
    slaveFutures: List[Future[Host]],
    serviceDescription: AmbariServiceDescription): Future[Service] = {
    for {
      cluster <- cluster_>
      master <- master_>
      slaves <- Future.sequence(slaveFutures)
      service <- serviceDescription.createService(cluster, master, slaves)
    } yield service
  }

  private def masterAndSlaves(hostFutures: List[Future[Host]]) = hostFutures match {
    case oneHost_> :: Nil => (oneHost_>, List(oneHost_>))
    case first_> :: restFutures => (first_>, restFutures)
    case _ => throw new IllegalArgumentException("Need at least one host")
  }

  private def installInOrder(serviceFutures: List[Future[Service]]): Future[List[Service]] = {
    def doInstall(
      installedServices_> : Future[List[Service]],
      service_> : Future[Service]): Future[List[Service]] = {
      for {
        installedServices <- installedServices_>
        service <- installAndStart(service_>)
      } yield service :: installedServices
    }
    serviceFutures.foldLeft(Future.successful(List[Service]()))(doInstall)
  }

  private def installAndStart(service_> : Future[Service]): Future[Service] = {
    for {
      service <- service_>
      installedService <- service.install()
      startedService <- installedService.start()
    } yield startedService
  }

  private def applyConfiguration(
    cluster_> : Future[Cluster],
    master_> : Future[Host],
    contributors: List[ConfigurationContributor]): Future[Cluster] = {
    for {
      cluster <- cluster_>
      master <- master_>
      _ <- Configurator.applyConfiguration(cluster, master, contributors)
    } yield cluster
  }

  private def addHosts(
    machineFutures: List[Future[MachineState]],
    cluster_> : Future[Cluster]): List[Future[Host]] = {
    for {
      machine_> <- machineFutures
    } yield for {
      cluster <- cluster_>
      machine <- machine_>
      host <- cluster.addHost(machine.hostname)
    } yield host
  }
}
