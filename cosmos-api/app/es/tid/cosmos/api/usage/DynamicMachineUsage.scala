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

package es.tid.cosmos.api.usage

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao._
import es.tid.cosmos.api.quota.{GlobalGroupQuotas, GuaranteedGroup}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}

/** Machine usage dynamically computed from a data store and the service manager. */
class DynamicMachineUsage(
    store: ProfileDataStore with GroupDataStore with ClusterDataStore,
    serviceManager: ServiceManager) extends MachineUsage {

  override def machinePoolSize = serviceManager.clusterNodePoolCount

  override def globalGroupQuotas: GlobalGroupQuotas[ProfileId] =
    store.withConnection { implicit c =>
      val membersByGroup = (for {
        group@GuaranteedGroup(_, _) <- store.group.list()
        members = store.profile.lookupByGroup(group).map(_.id)
      } yield group -> members).toMap
      GlobalGroupQuotas(membersByGroup)
    }

  override def usageByProfile(requestedClusterId: Option[ClusterId]): Map[ProfileId, Int] =
    store.withConnection { implicit c =>
      (for {
        g <- store.group.list().toSeq
        profile <- store.profile.lookupByGroup(g)
      } yield profile.id -> usedMachinesForActiveClusters(profile, requestedClusterId)).toMap
    }

  private def usedMachinesForActiveClusters(
      profile: CosmosProfile, requestedClusterId: Option[ClusterId]): Int =
    store.withConnection { implicit c =>
      (for (
        description <- listClusters(profile) if !isRequestedCluster(description, requestedClusterId)
      ) yield description.expectedSize).sum
    }

  private def isRequestedCluster(
      description: ClusterDescription, requestedClusterId: Option[ClusterId]): Boolean =
    requestedClusterId match {
      case Some(clusterId) => description.id == clusterId
      case None => false
    }

  private def listClusters(profile: CosmosProfile)(implicit c: store.Conn) = {
    val assignedClusters =  Set(store.cluster.ownedBy(profile.id): _*)
    for {
      assignment <- assignedClusters.toList
      description <- serviceManager.describeCluster(assignment.clusterId).toList
    } yield description
  }
}
