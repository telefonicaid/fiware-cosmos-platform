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

import scalaz._

import es.tid.cosmos.api.controllers.cluster.ClusterReference
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ClusterId}

/**
 * Class responsible for handling user quotas.
 *
 * @constructor           create a new profile-quotas handler.
 *                        ''Note:'' The constructor uses function and pass-by-name parameters to
 *                        allow loose-coupling while deferring their evaluation at the latest
 *                        possible moment.
 * @param machinePoolSize the total number of machines regardless of their usage
 * @param groups          the user groups
 * @param lookupByGroup   a function to lookup the users of a given group
 * @param listClusters    a function to get the all the clusters of a given user independently
 *                        of their state
 */
class ProfileQuotas(
  machinePoolSize: => Int,
  groups: => Set[Group],
  lookupByGroup: Group => Set[CosmosProfile],
  listClusters: CosmosProfile => Seq[ClusterReference]) {

  //TODO lookupByGroup -> listClusters could be simplified with a function
  // Group => List[Int] where Int represents the number of machines used by a group

  /**
   * Check if the given user can request the given number of machines based on their profile and
   * group quotas.
   *
   * @param profile            the user profile
   * @param size               the cluster size as a number of machines being requested for the user
   *                           (apart from the ones they already might have)
   * @param requestedClusterId the ID of the cluster for which the request is made.
   *                           This is to be provided when the cluster has already been registered
   *                           so that it can be filtered out when considering the existing clusters
   *                           machine usage
   * @return                   the validation result containing the requested cluster size if
   *                           validation was successful or the error messages in case of
   *                           validation failures
   */
  def withinQuota(
      profile: CosmosProfile,
      size: Int,
      requestedClusterId: Option[ClusterId] = None): ValidationNel[String, Int] = {

    new WithClusterFilter(requestedClusterId).withinQuota(profile, size)
  }

  /**
   * Internal implementation of `withinQuota` as a closure on the `requestedClusterId` which
   * is passed throughout the code so as to exclude it when considering existing clusters.
   *
   * @param requestedClusterId the ID of the cluster requested to be created. If it holds a value
   *                           then it is assumed that the cluster has already been registered and
   *                           it will be filtered out.
   */
  private class WithClusterFilter(requestedClusterId: Option[ClusterId]) {

    import scalaz.Scalaz._

    def withinQuota(profile: CosmosProfile, size: Int): ValidationNel[String, Int] = {
      val availableFromProfile = profile.quota - Quota(usedMachinesForActiveClusters(profile))
      val availableFromGroup = maxAvailableFromGroup(profile.group)
      val overallAvailable = Quota.min(availableFromProfile, availableFromGroup)

      val profileValidation = validate(availableFromProfile, size, "Profile quota exceeded")
      val groupValidation = validate(
        availableFromGroup, size, s"Quota exceeded for group [${profile.group.name}].")
      val overallValidation = validate(
        overallAvailable, size,
        s"You can request up to ${overallAvailable.toInt.getOrElse(0)} machine(s) at this point.")

      (profileValidation |@| groupValidation |@| overallValidation){(_, _, last) => last}
    }

    private def maxAvailableFromGroup(group: Group): Quota = {
      val usedByGroups = usedMachinesByGroups(groups)
      val maxQuota = GroupQuotas.maximumQuota(group, usedByGroups, machinePoolSize)
      maxQuota - Quota(usedByGroups(group))
    }

    private def validate(quota: Quota, size: Int, errorMessage: String): ValidationNel[String, Int] =
      if (quota.withinQuota(size)) size.successNel[String] else errorMessage.failureNel[Int]

    private def usedMachinesByGroups(groups: Set[Group]): Map[Group, Int] =
      (for {
        group <- groups
        groupProfiles = lookupByGroup(group)
        usedMachinesForGroup = groupProfiles.map(usedMachinesForActiveClusters).sum
      } yield group -> usedMachinesForGroup).toMap

    private def usedMachinesForActiveClusters(profile: CosmosProfile): Int =
      (for {
        clusterReference <- listClusters(profile)
        description = clusterReference.description if !isRequestedCluster(description)
      } yield description.expectedSize).sum

    private def isRequestedCluster(description: ClusterDescription): Boolean =
      requestedClusterId match {
        case Some(clusterId) => description.id == clusterId
        case None => false
      }
  }
}
