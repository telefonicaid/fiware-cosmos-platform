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


import org.mockito.BDDMockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.controllers.cluster.ClusterReference
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.platform.common.scalatest.matchers.ValidationMatchers
import es.tid.cosmos.servicemanager.clusters.ClusterDescription

class ProfileQuotasTest extends FlatSpec
    with MustMatchers with MockitoSugar with ValidationMatchers {

  "A user request exceeding their personal quota" must "not be allowed" in {
    val userGroup = GuaranteedGroup("A", FiniteQuota(3))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = FiniteQuota(3),
      usedMachinesByUser = 2,
      machinePool = 10
    )
    withConfig(context) { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must haveFailures(
        "Profile quota exceeded",
        "You can request up to 1 machine(s) at this point."
      )
    }
  }

  "A user request exceeding their minimum group quota" must "be allowed if machines available" in {
    /*
    +----+----+---+---+----+----+----+---+---+----+
    | 1  | 2  | 3 | 4 | 5  | 6  | 7  | 8 | 9 | 10 |
    +----+----+---+---+----+----+----+---+---+----+
    | A  | A  | B | B | NG | NG | NG | o | o | o  |
    | ur | ur | r | r | u  | u  | u  | o | o | o  |
    +----+----+---+---+----+----+----+---+---+----+
                                       X   X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(2))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 2,
      machinePool = 10
    )
    withConfig(context) { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must (beSuccessful and haveValidValue(2))
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machine(s) at this point."
      )
    }
  }

  "A user without personal quota" must "be allowed the max number of available machines" in {
    /*
    +----+----+---+---+---+----+----+----+---+----+
    | 1  | 2  | 3 | 4 | 5 | 6  | 7  | 8  | 9 | 10 |
    +----+----+---+---+---+----+----+----+---+----+
    | A  | A  | A | B | B | NG | NG | NG | o | o  |
    | ur | ur | r | r | r | u  | u  | u  | o | o  |
    +----+----+---+---+---+----+----+----+---+----+
                X                          X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(3))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 2,
      machinePool = 10
    )
    withConfig(context){ (quotas, profile) =>
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machine(s) at this point."
      )
    }
  }

  def withConfig(c: Context)
              (test: (ProfileQuotas, CosmosProfile) => Unit) {
    // for each group we need
    // 1. Create 1 profile or 1 + user for userGroup
    // 2. For each profile we need to create one cluster with the size of the group's usage
    // 3. Register the profiles
    // 4. assign clusters

    val groupProfiles = (for ((group, id) <- c.groupUsage.keys zip (1 to c.groupUsage.size)) yield {
      val quota = if (group == c.userGroup) c.personalMaxQuota else UnlimitedQuota
      val profile = CosmosProfile(
        id,
        state = Enabled,
        handle = s"handle$id",
        email = "user@example.com",
        group,
        quota,
        ApiCredentials.random(),
        keys = Nil
      )
      group -> Set(profile)
    }).toMap

    val groupClusters = c.groupUsage.map{ case (group, used) => {
      val description = mock[ClusterDescription]
      given(description.size).willReturn(used)
      val reference = mock[ClusterReference]
      given(reference.description).willReturn(description)
      groupProfiles(group).head -> List(reference)
    }}

    val quotas = new ProfileQuotas(
      machinePoolSize = c.machinePool,
      groups = c.groupUsage.keys.toSet,
      lookupByGroup = groupProfiles,
      listClusters = groupClusters
    )

    test(quotas, groupProfiles(c.userGroup).head)
  }

  case class Context(
    groupUsage: Map[Group, Int],
    userGroup: Group,
    personalMaxQuota: Quota,
    usedMachinesByUser: Int,
    machinePool: Int
  )
}
