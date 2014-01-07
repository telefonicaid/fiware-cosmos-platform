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

import es.tid.cosmos.servicemanager.clusters.ClusterId

/**
 * Class responsible for handling user quotas.
 *
 * @constructor create a new profile-quotas handler.
 *              ''Note:'' The constructor uses function and pass-by-name parameters to allow
 *              loose-coupling while deferring their evaluation at the latest possible moment.
 * @param dao   the DAO to use to retrieve information about machine usage
 */
class ProfileQuotas(dao: MachineUsageDao) {
  import scalaz.Scalaz._

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
    val daoWithFilter = dao.withClusterFilter(requestedClusterId)
    val availableFromProfile = profile.quota - Quota(daoWithFilter.usedMachinesForActiveClusters(profile))
    val availableFromGroup = maxAvailableFromGroup(profile.group, daoWithFilter)
    val overallAvailable = Quota.min(availableFromProfile, availableFromGroup)

    val profileValidation = validate(availableFromProfile, size, "Profile quota exceeded.")
    val groupValidationMessage = "Quota exceeded for %s.".format(
      if (profile.group == NoGroup) "users not belonging to any group"
      else s"group [${profile.group.name}]"
    )
    val groupValidation = validate(availableFromGroup, size, groupValidationMessage)
    val available = overallAvailable.toInt.getOrElse(0)
    val overallValidation = validate(overallAvailable, size,
      s"You can request up to $available machine${if (available != 1) "s" else ""} at this point.")

    (profileValidation |@| groupValidation |@| overallValidation){(_, _, last) => last}
  }

  private def maxAvailableFromGroup(group: Group, daoWithFilter: dao.WithClusterFilter): Quota = {
    val usedByGroups = daoWithFilter.usedMachinesByGroups
    val maxQuota = GroupQuotas.maximumQuota(group, usedByGroups, dao.machinePoolSize)
    maxQuota - Quota(usedByGroups(group))
  }

  private def validate(quota: Quota, size: Int, errorMessage: String): ValidationNel[String, Int] =
    if (quota.withinQuota(size)) size.successNel[String] else errorMessage.failureNel[Int]
}
