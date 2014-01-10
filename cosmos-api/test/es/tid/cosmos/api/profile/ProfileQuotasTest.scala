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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.api.quota._
import es.tid.cosmos.platform.common.scalatest.data.DataPicker
import es.tid.cosmos.platform.common.scalatest.matchers.ValidationMatchers
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.clusters.ClusterState.ActiveStates

class ProfileQuotasTest extends FlatSpec
    with MustMatchers with MockitoSugar with ValidationMatchers with DataPicker {

  override val seed: Int = 1

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
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must haveFailures(
        "Profile quota exceeded.",
        "You can request up to 1 machine at this point."
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
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must (beSuccessful and haveValidValue(2))
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machines at this point."
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
    context.ensureThat{ (quotas, profile) =>
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machines at this point."
      )
    }
  }

  "A quota" must "consider the expected size of a cluster in failed state " in {
    /*
    +----+----+---+----+----+----+----+---+---+----+
    | 1  | 2  | 3 | 4  | 5  | 6  | 7  | 8 | 9 | 10 |
    +----+----+---+----+----+----+----+---+---+----+
    | A  | A  | B | B  | NG | NG | NG | o | o | o  |  f0 = failed without any allocated machines
    | tr | ur | r | rt | u  | f0 | f1 | o | o | o  |  f1 = failed with 1 allocated machine
    +----+----+---+----+----+----+----+---+---+----+
      X                       X         X   X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(2))
    val groupB = GuaranteedGroup("B", FiniteQuota(2))
    val groupUsage = Map[Group, Int](
      userGroup -> 1,
      groupB -> 0,
      NoGroup -> 1
    )
    val terminatedClusters = Map[Group, List[ClusterDescription]](
      userGroup -> List(clusterDescription(1, Terminated)),
      groupB -> List(clusterDescription(1, Terminated)),
      NoGroup -> List(
        clusterDescription(
          1, Failed("Cluster with allocated machine blew up"), withMachineInfo = true),
        clusterDescription(
          1, Failed("Cluster without allocated machines blew up"), withMachineInfo = false)
      )
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 1,
      machinePool = 10,
      extraClusters = terminatedClusters
    )
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must (beSuccessful and haveValidValue(2))
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must (beSuccessful and haveValidValue(4))
      quotas.withinQuota(profile, 5) must (beSuccessful and haveValidValue(5))
      quotas.withinQuota(profile, 6) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 5 machines at this point."
      )
    }
  }

  it must "not consider the requested cluster if it has already been registered" in {
    /*
    +----+----+----+----+----+----+----+----+----+----+
    | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 |
    +----+----+----+----+----+----+----+----+----+----+
    | NG | NG | NG | NG | NG | NG | NG | NG | NG | NG |
    | o  | o  | o  | o  | o  | o  | o  | o  | o  | o  |
    +----+----+----+----+----+----+----+----+----+----+
      X    X    X    X    X    X    X    X    X    X
     */
    val requestedCluster = clusterDescription(10, Provisioning)
    val context = Context(
      groupUsage = Map(NoGroup -> 0),
      userGroup = NoGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 0,
      machinePool = 10,
      extraClusters = Map(NoGroup -> List(requestedCluster))
    )
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 10, requestedClusterId = None) must haveFailures(
        "Quota exceeded for users not belonging to any group.",
        "You can request up to 0 machines at this point."
      )
      quotas.withinQuota(
        profile, 10, requestedClusterId = Some(requestedCluster.id)) must
        (beSuccessful and haveValidValue(10))
    }
  }

  /* Regression test for issue https://pdihub.hi.inet/Cosmos/cosmos-platform/issues/1506 */
  it must "correctly sum number of used machines among profiles of same group " +
    "even if some profiles have the same number of used machines" in {
    val aGroup = GuaranteedGroup("groupOf2", Quota(12))
    val (noGroupUser1, noGroupUser2) = (
      ProfileWithClusters(1, NoGroup, Seq(2, 1, 1)),
      ProfileWithClusters(2, NoGroup, Seq(2, 2))
      // 8 used machines
    )
    val (groupOf2User1, groupOf2User2) = (
      ProfileWithClusters(3, aGroup, Seq(3, 2)),
      ProfileWithClusters(4, aGroup, Seq(2, 1, 2))
      // 10 used machines
   )
    val groupContexts: GroupContexts = Map[Group, Set[ProfileWithClusters]](
      NoGroup -> Set(noGroupUser1, noGroupUser2),
      aGroup -> Set(groupOf2User1, groupOf2User2)
    )
    val dao = new MockedMachineUsageDao(
      machinePoolSize = 22,
      groups = Set(NoGroup, aGroup),
      groupsToProfiles = groupContexts.groupsToProfiles,
      profilesToClusters = groupContexts.profilesToClusters
    )
    val quotas = new ProfileQuotas(dao)

    // 22 - 18 used = 4 available
    // groupOf2 10 used 12 reserved.
    // 2 available for NoGroup
    // 4 available for groupOf2
    quotas.withinQuota(noGroupUser1.profile, 2) must (beSuccessful and haveValidValue(2))
    quotas.withinQuota(noGroupUser1.profile, 3) must haveFailures(
      "Quota exceeded for users not belonging to any group.",
      "You can request up to 2 machines at this point."
    )

    quotas.withinQuota(groupOf2User1.profile, 4) must (beSuccessful and haveValidValue(4))
    quotas.withinQuota(groupOf2User1.profile, 5) must haveFailures(
      "Quota exceeded for group [groupOf2].",
      "You can request up to 4 machines at this point."
    )
  }

  case class Context(
    groupUsage: Map[Group, Int],
    userGroup: Group,
    personalMaxQuota: Quota,
    usedMachinesByUser: Int,
    machinePool: Int,
    extraClusters: Map[Group, List[ClusterDescription]] = Map.empty
  ) {

    /**
     * Execute a test on the given context as configuration.
     *
     * 1. Create 1 profile or 1 + user for userGroup
     * 2. For each profile we need to create one cluster with the size of the group's usage
     * 3. Register the profiles
     * 4. assign clusters
     *
     * @param test the test as a function receiving the quotas manager and the profile to check
     */
    def ensureThat(test: (ProfileQuotas, CosmosProfile) => Unit) {
      import scalaz.Scalaz._

      val groupContexts: GroupContexts =
        (for (((group, usage), index) <- groupUsage.zipWithIndex) yield {
          val profileQuota = if (group == userGroup) personalMaxQuota else UnlimitedQuota
          group -> Set(ProfileWithClusters(index, group, Seq(usage), profileQuota))
        }).toMap

      val extraProfilesToClusters = extraClusters.mapKeys(groupContexts.groupsToProfiles(_).head)

      val dao = new MockedMachineUsageDao(
        machinePoolSize = machinePool,
        groups = groupUsage.keys.toSet,
        groupsToProfiles = groupContexts.groupsToProfiles,
        profilesToClusters = groupContexts.profilesToClusters |+| extraProfilesToClusters
      )
      val quotas = new ProfileQuotas(dao)

      test(quotas, groupContexts.groupsToProfiles(userGroup).head)
    }
  }

  def clusterDescription(
    size: Int,
    state: ClusterState,
    withMachineInfo: Boolean = true): ClusterDescription = {

    def maybeMaster: Option[HostDetails] = if (withMachineInfo) Some(mock[HostDetails]) else None
    def maybeSlaves: Seq[HostDetails] =
      if (withMachineInfo) Seq.empty else (1 to size - 1).map(_ => mock[HostDetails])
    def maybeUsers: Option[Set[ClusterUser]] = if (withMachineInfo) Some(Set.empty) else None

    val clusterId = ClusterId()
    val description = ImmutableClusterDescription(
      id = clusterId,
      name = clusterId.toString,
      size = size,
      nameNode = None,
      state = state,
      master = maybeMaster,
      slaves = maybeSlaves,
      users = maybeUsers,
      services = Set.empty
    )
    description
  }

  private type GroupContexts = Map[Group, Set[ProfileWithClusters]]

  private case class ProfileWithClusters(
      profileId: Long, group: Group, clusterSizes: Seq[Int], profileQuota: Quota = UnlimitedQuota) {

    val profile: CosmosProfile = CosmosProfile(
      profileId,
      state = Enabled,
      handle = s"handle$profileId",
      email = "user@example.com",
      ApiCredentials.random(),
      keys = Nil,
      group,
      quota = profileQuota,
      capabilities = UntrustedUserCapabilities
    )

    val clusters: Seq[ClusterDescription] = clusterSizes.map(
      size => clusterDescription(size, pickAnyOne(ActiveStates))
    )
  }

  implicit private class GroupsToProfileWithClustersOps(thiz: GroupContexts) {

    val groupsToProfiles: Map[Group, Set[CosmosProfile]] = thiz.mapValues(pwc => pwc.map(_.profile))

    val profilesToClusters: Map[CosmosProfile, List[ClusterDescription]] =
      thiz.values.flatten.map(pwc => pwc.profile -> pwc.clusters.toList).toMap
  }
}
