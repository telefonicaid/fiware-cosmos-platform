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

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.rest.{ClusterProvisioner, Cluster}
import es.tid.cosmos.platform.ial.InfrastructureProvider
import es.tid.cosmos.servicemanager.ambari.services.Hdfs
import java.net.URI

/**
 * Trait for refreshing cluster state by picking up clusters from
 * Ambari that are unregistered.
 */
trait Refreshing extends Refreshable with Logging {
  protected def infrastructureProvider: InfrastructureProvider
  protected def provisioner: ClusterProvisioner
  protected def refreshGracePeriod: Int
  protected def clusterIds: Seq[ClusterId]
  protected def registerCluster(description: MutableClusterDescription)

  /**
   * Refresh the clusters state by querying the provisioner for unregistered clusters
   * and registering them.
   *
   * @return the completion future of the refresh task
   */
  override def refresh() : Future[Unit] =
    for {
      names <- provisioner.listClusterNames
      unregisteredNames = names.filterNot(name => clusterIds.contains(ClusterId(name)))
      unregisteredClusters <- Future.traverse(unregisteredNames)(provisioner.getCluster)
      _ = logger.info(s"Found ${unregisteredClusters.size} unregistered cluster(s).")
      _ <- Future.traverse(unregisteredClusters)(register)
    } yield ()

  private def stateFrom(serviceStates: Seq[String]) =
    if (serviceStates.forall(_ == "STARTED")) Running else Provisioning

  private def state(cluster: Cluster): Future[ClusterState] = for (
    services <- Future.traverse(cluster.serviceNames)(cluster.getService)
  ) yield stateFrom(services.map(_.state))

  private def resolveState(cluster: Cluster, isWithGracePeriod : Boolean = true): Future[ClusterState] = {
    val state_> = state(cluster)
    state_>.flatMap {
      case Running => state_>
      case Provisioning => if (isWithGracePeriod) {
        logger.warn(s"Found unregistered cluster [${cluster.name}}] in Provisioning state. " +
          s"Waiting ${refreshGracePeriod/1000} second(s) to see if it changes.")
        Thread.sleep(refreshGracePeriod)
        resolveState(cluster, isWithGracePeriod = false)
      } else throw new IllegalStateException(
        s"Timed out waiting for cluster [${cluster.name}] to finish provisioning")
      case other@_ => throw new IllegalStateException(
        s"Oops! Was not expecting the cluster [${cluster.name}}] in state [$other]")
    }
  }

  private def register(cluster: Cluster) = {
    val clusterState_> = resolveState(cluster)
    val machines_> = infrastructureProvider.assignedMachines(cluster.hostNames)
    val nameNode_> = for {
      host <- ServiceMasterExtractor.getServiceMaster(cluster, Hdfs)
    } yield new URI(s"hdfs://${host.name}:${Hdfs.nameNodeHttpPort}")
    registerCluster(
      new MutableClusterDescription(
        ClusterId(cluster.name), cluster.name, cluster.hostNames.size,
        clusterState_>, machines_>, nameNode_>))
    clusterState_>
  }
}
