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

import es.tid.cosmos.platform.common.scalatest.matchers.ValidationMatchers

class QuotaContextTest extends FlatSpec with MustMatchers with ValidationMatchers {

  "A user request exceeding their personal quota" must "not be allowed" in {
    Scenario(poolSize = 10, groups = Map(
      GuaranteedGroup("A", Quota(3)) -> Seq(
        Profile(id = "user0", usage = 2, Quota(3))
      ),
      GuaranteedGroup("B", Quota(2)) -> Seq(),
      NoGroup -> Seq(
        Profile(id = "user1", usage = 3)
      )
    )).ensureThat { (context, profiles) =>
      context.withinQuota(profiles("user0"), 1) must (beSuccessful and haveValidValue(1))
      context.withinQuota(profiles("user0"), 2) must haveFailures(
        "Profile quota exceeded.",
        "You can request up to 1 machine at this point."
      )
    }
  }

  "A user request exceeding available resources" must "not be allowed" in {
    Scenario(poolSize = 10, groups = Map(
      NoGroup -> Seq(
        Profile(id = "user0", usage = 2),
        Profile(id = "user1", usage = 3)
      )
    )).ensureThat { (context, profiles) =>
      context.withinQuota(profiles("user0"), 5) must (beSuccessful and haveValidValue(5))
      context.withinQuota(profiles("user0"), 6) must haveFailures(
        "Quota exceeded for users not belonging to any group.",
        "You can request up to 5 machines at this point."
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
    Scenario(poolSize = 10, groups = Map(
      GuaranteedGroup("A", Quota(2)) -> Seq(
        Profile(id = "user0", usage = 2)
      ),
      GuaranteedGroup("B", Quota(2)) -> Seq(),
      NoGroup -> Seq(
        Profile(id = "user1", usage = 3)
      )
    )).ensureThat { (context, profiles) =>
      context.withinQuota(profiles("user0"), 1) must (beSuccessful and haveValidValue(1))
      context.withinQuota(profiles("user0"), 2) must (beSuccessful and haveValidValue(2))
      context.withinQuota(profiles("user0"), 3) must (beSuccessful and haveValidValue(3))
      context.withinQuota(profiles("user0"), 4) must haveFailures(
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
    Scenario(poolSize = 10, groups = Map(
      GuaranteedGroup("A", Quota(3)) -> Seq(
        Profile(id = "user0", usage = 2)
      ),
      GuaranteedGroup("B", Quota(2)) -> Seq(),
      NoGroup -> Seq(
        Profile(id = "user1", usage = 3)
      )
    )).ensureThat { (context, profiles) =>
      context.withinQuota(profiles("user0"), 3) must (beSuccessful and haveValidValue(3))
      context.withinQuota(profiles("user0"), 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machines at this point."
      )
    }
  }

  "The minimum group quota" must "be guaranteed for all groups" in {
    val ex = evaluating {
      Scenario(poolSize = 10, groups = Map(
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
    "even if some profiles have the same number of used machines" in {

    Scenario(poolSize = 22, groups = Map(
      NoGroup -> Seq(
        Profile(id = "user1", usage = 4),
        Profile(id = "user2", usage = 4)
        // 8 used machines
      ),
      GuaranteedGroup("A", Quota(12)) -> Seq(
        Profile(id = "user3", usage = 5),
        Profile(id = "user4", usage = 5)
        // 10 used machines
      )
    )).ensureThat { (context, profiles) =>
      // 22 - 18 used = 4 available
      // group A: 10 used 12 reserved.

      // 2 available for NoGroup
      context.withinQuota(profiles("user1"), 2) must (beSuccessful and haveValidValue(2))
      context.withinQuota(profiles("user1"), 3) must haveFailures(
        "Quota exceeded for users not belonging to any group.",
        "You can request up to 2 machines at this point."
      )

      // 4 available for group A
      context.withinQuota(profiles("user3"), 4) must (beSuccessful and haveValidValue(4))
      context.withinQuota(profiles("user3"), 5) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 4 machines at this point."
      )
    }
  }

  case class Profile(id: String, usage: Int = 0, quota: Quota = UnlimitedQuota) {
    def quotaConsumer(quotaGroup: Group) = new QuotaConsumer[String] {
      override val id = Profile.this.id
      override val quota = Profile.this.quota
      override val group = quotaGroup
    }
  }

  case class Scenario(poolSize: Int, groups: Map[Group, Seq[Profile]]) {

    private val groupQuotas = GlobalGroupQuotas(for {
      (group: GuaranteedGroup, profiles) <- groups
      members = profiles.map(_.id).toSet
    } yield group -> members)

    private val context = QuotaContext(
      poolSize = poolSize,
      groupQuotas = groupQuotas,
      usageByProfile = (for {
        profile <- groups.values.flatten
      } yield profile.id -> profile.usage).toMap
    )

    private val cosmosProfiles = for {
      (group, profiles)<- groups
      profile <- profiles
    } yield profile.id -> profile.quotaConsumer(group)

    def ensureThat(
        test: (QuotaContext[String], Map[String, QuotaConsumer[String]]) => Unit) {
      test(context, cosmosProfiles)
    }
  }
}
