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
