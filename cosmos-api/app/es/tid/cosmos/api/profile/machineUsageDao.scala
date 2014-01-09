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

import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}

/** Definition of a DAO responsible for retrieving machine usage information. */
trait MachineUsageDao {

  /** The total number of machines in the machine pool */
  def machinePoolSize: Int

  /** Get the number of used machines for each group.
    *
    * @param requestedClusterId the optional ID of the cluster requested to be created.
    *                           When this is available it will be filtered out from the
    *                           usage calculation since the cluster will have been registered
    *                           without having been created yet.
    * @return                   the number of used machines per group
    */
  def usedMachinesByGroups(requestedClusterId: Option[ClusterId]): Map[Group, Int]

  /** Get the number of machines used by all the ''active'' clusters of a given user profile.
    * A cluster is considered active if its state is within the set of
    * [[es.tid.cosmos.servicemanager.clusters.ClusterState.ActiveStates]].
    *
    * @param profile            the user profile for whom to get the number of used machines
    * @param requestedClusterId the optional ID of the cluster requested to be created.
    *                           When this is available it will be filtered out from the
    *                           usage calculation since the cluster will have been registered
    *                           without having been created yet.
    * @return the total number of used machines for the profile's active cluster.
    */
  def usedMachinesForActiveClusters(
      profile: CosmosProfile, requestedClusterId: Option[ClusterId]): Int
}

/** Machine Usage DAO that combines the information of the
  * [[es.tid.cosmos.api.profile.CosmosProfileDao]] and
  * [[es.tid.cosmos.servicemanager.ServiceManager]] in order to calculate the machine usage at a
  * given time.
  *
  * @constructor create a new usage DAO that consolidates the information from
  *              a profile DAO and a service manager to produce the machine usage information
  * @param profileDao the profile DAO
  * @param serviceManager the service manager
  */
class CosmosMachineUsageDao(
    profileDao: CosmosProfileDao, serviceManager: ServiceManager) extends MachineUsageDao {

  override def machinePoolSize = serviceManager.clusterNodePoolCount

  override def usedMachinesByGroups(requestedClusterId: Option[ClusterId]): Map[Group, Int] =
    profileDao.withConnection { implicit c =>
      val groupsToUsedMachines: Set[(Group, Int)] = for {
        group <- profileDao.getGroups
        groupProfiles: Set[CosmosProfile] = profileDao.lookupByGroup(group)
        /*
         * groupProfiles.toSeq necessary so that map transformation does not give a set which
         * would discard machine sizes of the same number.
         */
        usedMachinesForGroupProfiles: Seq[Int] = groupProfiles.toSeq.map(
          _usedMachinesForActiveClusters(_, requestedClusterId))
        usedMachinesForGroup = usedMachinesForGroupProfiles.sum
      } yield group -> usedMachinesForGroup

      groupsToUsedMachines.toMap
    }

  override def usedMachinesForActiveClusters(
      profile: CosmosProfile, requestedClusterId: Option[ClusterId]): Int =
    profileDao.withConnection {
      c => _usedMachinesForActiveClusters(profile, requestedClusterId)(c)
    }

  private def _usedMachinesForActiveClusters(
      profile: CosmosProfile, requestedClusterId: Option[ClusterId])
                                            (implicit c: profileDao.Conn): Int =
    (for (
      description <- listClusters(profile) if !isRequestedCluster(description, requestedClusterId))
      yield description.expectedSize).sum

  private def isRequestedCluster(
      description: ClusterDescription, requestedClusterId: Option[ClusterId]): Boolean =
    requestedClusterId match {
      case Some(clusterId) => description.id == clusterId
      case None => false
    }

  private def listClusters(profile: CosmosProfile)(implicit c: profileDao.Conn) = {
    val assignedClusters =  Set(profileDao.clustersOf(profile.id): _*)
    for {
      assignment <- assignedClusters.toList
      description <- serviceManager.describeCluster(assignment.clusterId).toList
    } yield description
  }
}
