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

  /** Access the DAO functionality while filtering out the requested cluster in the process.
    * This is useful for usage calculations where the requested cluster has already been
    * registered in the system but should not be taken into account since it has not been
    * provisioned yet.
    *
    * @param requestedClusterId the ID of the requested cluster to be filtered out
    * @return the DAO functionality with the applied filter.
    */
  def withClusterFilter(requestedClusterId: Option[ClusterId]): WithClusterFilter

  /** Access the DAO functionality without filtering any clusters.
    *
    * @return the DAO functionality without any filter
    */
  final def withoutClusterFilter: WithClusterFilter = withClusterFilter(None)

  /** The DAO functionality that supports filtering. */
  trait WithClusterFilter {
    def usedMachinesByGroups: Map[Group, Int]
    def usedMachinesForActiveClusters(profile: CosmosProfile): Int
  }
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

  override def withClusterFilter(requestedClusterId: Option[ClusterId]) =
    new CosmosWithClusterFilter(requestedClusterId)

  private def listClusters(profile: CosmosProfile)(implicit c: profileDao.Conn) = {
    val assignedClusters =  Set(profileDao.clustersOf(profile.id): _*)
    for {
      assignment <- assignedClusters.toList
      description <- serviceManager.describeCluster(assignment.clusterId).toList
    } yield description
  }

  /** Filtered operations for [[es.tid.cosmos.api.profile.CosmosMachineUsageDao]].
    *
    * @param requestedClusterId the ID of the cluster to filter out
    */
  class CosmosWithClusterFilter(requestedClusterId: Option[ClusterId]) extends WithClusterFilter {

    override def usedMachinesByGroups: Map[Group, Int] = profileDao.withConnection { implicit c =>
      val groupsToUsedMachines: Set[(Group, Int)] = for {
        group <- profileDao.getGroups
        groupProfiles: Set[CosmosProfile] = profileDao.lookupByGroup(group)
        /*
         * groupProfiles.toSeq necessary so that map transformation does not give a set which
         * would discard machine sizes of the same number.
         */
        usedMachinesForGroupProfiles: Seq[Int] = groupProfiles.toSeq.map(
          _usedMachinesForActiveClusters)
        usedMachinesForGroup = usedMachinesForGroupProfiles.sum
      } yield group -> usedMachinesForGroup

      groupsToUsedMachines.toMap
    }

    override def usedMachinesForActiveClusters(profile: CosmosProfile): Int =
      profileDao.withConnection { c => _usedMachinesForActiveClusters(profile)(c) }

    private def _usedMachinesForActiveClusters(profile: CosmosProfile)
                                              (implicit c: profileDao.Conn): Int =
      (for (description <- listClusters(profile) if !isRequestedCluster(description))
        yield description.expectedSize).sum

    private def isRequestedCluster(description: ClusterDescription): Boolean =
      requestedClusterId match {
        case Some(clusterId) => description.id == clusterId
        case None => false
      }
  }
}
