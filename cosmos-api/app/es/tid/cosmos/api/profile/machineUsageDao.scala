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

package es.tid.cosmos.api.profile

import es.tid.cosmos.api.profile.dao.CosmosDao
import es.tid.cosmos.api.quota.{GuaranteedGroup, GlobalGroupQuotas}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}

/** Definition of a DAO responsible for retrieving machine usage information. */
trait MachineUsageDao {

  /** The total number of machines in the machine pool */
  def machinePoolSize: Int

  /** Get the system-wide quotas at a given point in time. */
  def globalGroupQuotas: GlobalGroupQuotas[ProfileId]

  /** Aggregates resource usage by user profile.
    *
    * @param requestedClusterId the currently provisioning cluster to be filtered out or none
    */
  def usageByProfile(requestedClusterId: Option[ClusterId]): Map[ProfileId, Int]
}

/** Machine Usage DAO that combines the information of the
  * [[es.tid.cosmos.api.profile.dao.CosmosDao]] and
  * [[es.tid.cosmos.servicemanager.ServiceManager]] in order to calculate the machine usage at a
  * given time.
  *
  * @constructor create a new usage DAO that consolidates the information from
  *              a profile DAO and a service manager to produce the machine usage information
  * @param profileDao the profile DAO
  * @param serviceManager the service manager
  */
class CosmosMachineUsageDao(
    profileDao: CosmosDao, serviceManager: ServiceManager) extends MachineUsageDao {

  override def machinePoolSize = serviceManager.clusterNodePoolCount

  override def globalGroupQuotas: GlobalGroupQuotas[ProfileId] =
    profileDao.store.withConnection { implicit c =>
      val membersByGroup = (for {
        group@GuaranteedGroup(_, _) <- profileDao.group.list()
        members = profileDao.profile.lookupByGroup(group).map(_.id)
      } yield group -> members).toMap
      GlobalGroupQuotas(membersByGroup)
    }

  override def usageByProfile(requestedClusterId: Option[ClusterId]): Map[ProfileId, Int] =
    profileDao.store.withConnection { implicit c =>
      (for {
        g <- profileDao.group.list().toSeq
        profile <- profileDao.profile.lookupByGroup(g)
      } yield profile.id -> usedMachinesForActiveClusters(profile, requestedClusterId)).toMap
    }

  private def usedMachinesForActiveClusters(
      profile: CosmosProfile, requestedClusterId: Option[ClusterId]): Int =
    profileDao.store.withConnection { implicit c =>
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

  private def listClusters(profile: CosmosProfile)(implicit c: profileDao.Conn) = {
    val assignedClusters =  Set(profileDao.cluster.ownedBy(profile.id): _*)
    for {
      assignment <- assignedClusters.toList
      description <- serviceManager.describeCluster(assignment.clusterId).toList
    } yield description
  }
}
