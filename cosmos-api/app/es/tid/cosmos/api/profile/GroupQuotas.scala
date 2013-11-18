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

import es.tid.cosmos.api.profile.Quota.max

/**
  * Object responsible for calculating machine quotas for groups.
  */
object GroupQuotas {

  /**
    * Get the maximum possible quota for a given group.
    * This works by consulting the total number of machines along with the quota guarantees for each
    * group ensuring that the maximum quota returned does not jeopardise the guarantees made.
    *
    * @param group                 the group for which to get the maximum quota
    * @param groupsAndUsedMachines the already used machines of each group
    * @param machinePoolSize       the total number of machines regardless of whether they are used
    *                              or not
    * @return                      the quota representing the maximum number of machines the group
    *                              can have at this point in time
    */
  def maximumQuota(
      group: Group, groupsAndUsedMachines: Map[Group, Int], machinePoolSize: Int) : Quota = {

    val groups = groupsAndUsedMachines.keySet
    val machinePoolQuota = Quota(machinePoolSize)
    requirePoolCanMeetMinimumQuota(groups, machinePoolQuota)
    require(groups.contains(group))
    val reservedByGroups = for ((aGroup, machinesUsed) <- groupsAndUsedMachines)
      yield aGroup -> max(aGroup.minimumQuota, Quota(machinesUsed))
    val totalReserved = reservedByGroups.values.reduce(_ + _)
    val freeForAnyone = machinePoolQuota - totalReserved
    val maximumQuota = max(Quota(groupsAndUsedMachines(group)), group.minimumQuota) + freeForAnyone
    maximumQuota
  }

  private def requirePoolCanMeetMinimumQuota(groups: Set[Group], poolQuota: Quota) {
    val totalGuaranteedQuota = groups.map(_.minimumQuota).reduce(_ + _)
    require(
      poolQuota.withinQuota(totalGuaranteedQuota),
      "There are not enough machines available to meet the guaranteed minimum group quotas"
    )
  }
}
