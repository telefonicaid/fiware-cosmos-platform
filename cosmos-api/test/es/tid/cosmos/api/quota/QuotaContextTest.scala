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

package es.tid.cosmos.api.quota

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.ValidationMatchers

class QuotaContextTest extends FlatSpec with MustMatchers with ValidationMatchers {

  "A user request exceeding their personal quota" must "not be allowed" in new Scenario(Map(
    GuaranteedGroup("A", Quota(3)) -> Seq(
      Profile(id = "user0", usage = 2, Quota(3))
    ),
    GuaranteedGroup("B", Quota(2)) -> Seq(),
    NoGroup -> Seq(
      Profile(id = "user1", usage = 3)
    )
  )) {
    context.availableForUser(profiles("user0")) must be (1)
    context.withinQuota(profiles("user0"), 1) must (beSuccessful and haveValidValue(1))
    context.withinQuota(profiles("user0"), 2) must haveFailures(
      "Profile quota exceeded.",
      "You can request up to 1 machine at this point."
    )
  }

  "A user request exceeding available resources" must "not be allowed" in new Scenario(Map(
    NoGroup -> Seq(
      Profile(id = "user0", usage = 2),
      Profile(id = "user1", usage = 3)
    )
  )) {
    context.availableForUser(profiles("user0")) must be (5)
    context.withinQuota(profiles("user0"), 5) must (beSuccessful and haveValidValue(5))
    context.withinQuota(profiles("user0"), 6) must haveFailures(
      "Quota exceeded for users not belonging to any group.",
      "You can request up to 5 machines at this point."
    )
  }

  "A user request exceeding their minimum group quota" must "be allowed if machines available" in
    new Scenario(Map(
      GuaranteedGroup("A", Quota(2)) -> Seq(
        Profile(id = "user0", usage = 2)
      ),
      GuaranteedGroup("B", Quota(2)) -> Seq(),
      NoGroup -> Seq(
        Profile(id = "user1", usage = 3)
      )
    )) {
      context.availableForUser(profiles("user0")) must be (3)
      context.withinQuota(profiles("user0"), 1) must (beSuccessful and haveValidValue(1))
      context.withinQuota(profiles("user0"), 2) must (beSuccessful and haveValidValue(2))
      context.withinQuota(profiles("user0"), 3) must (beSuccessful and haveValidValue(3))
      context.withinQuota(profiles("user0"), 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machines at this point."
      )
    }

  "A user without personal quota" must "be allowed the max number of available machines" in
    new Scenario(Map(
      GuaranteedGroup("A", Quota(3)) -> Seq(
        Profile(id = "user0", usage = 2)
      ),
      GuaranteedGroup("B", Quota(2)) -> Seq(),
      NoGroup -> Seq(
        Profile(id = "user1", usage = 3)
      )
    )) {
      context.availableForUser(profiles("user0")) must be (3)
      context.withinQuota(profiles("user0"), 3) must (beSuccessful and haveValidValue(3))
      context.withinQuota(profiles("user0"), 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machines at this point."
      )
    }

  "The minimum group quota" must "be guaranteed for all groups" in {
    val ex = evaluating {
      Scenario(Map(
        GuaranteedGroup("A", Quota(5)) -> Seq(
          Profile(id = "user0", usage = 2)
        ),
        GuaranteedGroup("B", Quota(5)) -> Seq(),
        NoGroup -> Seq(
          Profile(id = "user1", usage = 3)
        )
      ))
    } must produce [IllegalArgumentException]
    ex.toString must include ("There are not enough machines available (deficit of 3)")
  }


  /* Regression test for issue https://pdihub.hi.inet/Cosmos/cosmos-platform/issues/1506 */
  it must "correctly sum number of used machines among profiles of same group " +
    "even if some profiles have the same number of used machines" in new Scenario(
      poolSize = 22,
      groups = Map(
        NoGroup -> Seq(
          Profile(id = "user1", usage = 4),
          Profile(id = "user2", usage = 4)
        ),
        GuaranteedGroup("A", Quota(12)) -> Seq(
          Profile(id = "user3", usage = 5),
          Profile(id = "user4", usage = 5)
        )
      )) {
        // 22 - 4 - 4 - 5 - 5 = 4 available
        // group A: 10 used 12 reserved.
        context.usageByGroup(group("A")) must be (10)

        // 2 available for NoGroup
        context.availableForUser(profiles("user1")) must be (2)
        context.withinQuota(profiles("user1"), 2) must (beSuccessful and haveValidValue(2))
        context.withinQuota(profiles("user1"), 3) must haveFailures(
          "Quota exceeded for users not belonging to any group.",
          "You can request up to 2 machines at this point."
        )

        // 4 available for group A
        context.availableForUser(profiles("user3")) must be (4)
        context.withinQuota(profiles("user3"), 4) must (beSuccessful and haveValidValue(4))
        context.withinQuota(profiles("user3"), 5) must haveFailures(
          "Quota exceeded for group [A].",
          "You can request up to 4 machines at this point."
        )
      }

  val groupQuotasContext = Scenario(
    poolSize = 15,
    groups = Map(
      NoGroup -> Seq(
        Profile(id = "userNG", usage = 4)
      ),
      GuaranteedGroup("A", Quota(8)) -> Seq(
        Profile(id = "userA1", usage = 3),
        Profile(id = "userA2", usage = 2)
      ),
      GuaranteedGroup("B", Quota(2)) -> Seq(
        Profile(id = "userB1", usage = 2),
        Profile(id = "userB2", usage = 0)
      )
    )
  ).context

  "The group minimum quota" must "be feasible when it decreases" in {
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("A", Quota(8))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("A", Quota(2))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("A", EmptyQuota)) must be ('success)

    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("B", Quota(2))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("B", Quota(1))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("B", EmptyQuota)) must be ('success)
  }

  it must "be feasible when it increases and there is unreserved capacity" in {
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("A", Quota(9))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("A", Quota(10))) must be ('failure)

    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("B", Quota(3))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("B", Quota(4))) must be ('failure)
  }

  "A new group's minimum quota" must
    "be feasible when it less or equal to the unreserved capacity" in {
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("C", EmptyQuota)) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("C", Quota(1))) must be ('success)
    groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("C", Quota(2))) must be ('failure)
  }

  "A failed group quota message" must "indicate the maximum possible guaranteed quota" in {
    val result = groupQuotasContext.isGroupQuotaFeasible(GuaranteedGroup("C", Quota(100)))
    result must haveFailure("Group C can have a minimum quota of up to FiniteQuota(1).")
  }

  case class Profile(id: String, usage: Int = 0, quota: Quota = UnlimitedQuota) {
    def quotaConsumer(quotaGroup: Group) = new QuotaConsumer[String] {
      override val id = Profile.this.id
      override val quota = Profile.this.quota
      override val group = quotaGroup
    }
  }

  case class Scenario(groups: Map[Group, Seq[Profile]], poolSize: Int = 10) {

    private val groupQuotas = GlobalGroupQuotas(for {
      (group: GuaranteedGroup, profiles) <- groups
      members = profiles.map(_.id).toSet
    } yield group -> members)

    val context = QuotaContext(
      poolSize = poolSize,
      groupQuotas = groupQuotas,
      usageByProfile = (for {
        profile <- groups.values.flatten
      } yield profile.id -> profile.usage).toMap
    )

    val profiles = for {
      (group, groupProfiles)<- groups
      profile <- groupProfiles
    } yield profile.id -> profile.quotaConsumer(group)

    def group(name: String) = groups.keys.find(_.name == name).get
  }
}
