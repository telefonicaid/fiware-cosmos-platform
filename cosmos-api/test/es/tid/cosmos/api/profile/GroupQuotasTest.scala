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

import es.tid.cosmos.api.profile.GroupQuotas.maximumQuota

class GroupQuotasTest extends FlatSpec with MustMatchers {

   /* Test diagram legend.
    *
    * u   = used by used
    * r   = reserved by group
    * ur  = reserved by group and used by a group user
    * o   = empty, i.e. no group, not used, not reserved
    * A,B = groups A and B
    * NG  = NoGroup
    */

  "The maximum quota when only one group" must "be the total number of machines in the pool" in {
    /*
    +----+---+---+---+---+
    | 1  | 2 | 3 | 4 | 5 |
    +----+---+---+---+---+
    | A  | A | A | o | o |
    | ur | r | r | o | o |
    +----+---+---+---+---+
      a    a   a   a   a    = 5 max
     */
    val group = GuaranteedGroup("A", minimumQuota = FiniteQuota(3))
    val usedMachines: Map[Group, Int] = Map(group -> 1)
    val poolSize = 5
    val maxQuota = maximumQuota(group, usedMachines, poolSize)
    maxQuota.withinQuota(group.minimumQuota) must be (true)
    maxQuota must be (FiniteQuota(poolSize))
  }

  "Groups" must "be invalid if their combined minimum quota exceeds the machine pool size" in {
    val usedMachines = Map[Group, Int](
      GuaranteedGroup("A", FiniteQuota(3)) -> 0,
      GuaranteedGroup("B", FiniteQuota(4)) -> 0,
      NoGroup -> 0
    )
    evaluating(maximumQuota(NoGroup, usedMachines, machinePoolSize = 2)) must
      produce [IllegalArgumentException]
  }

  "The maximum quota without groups" must "be the total number of machines in the pool" in {
    /*
    +----+---+---+---+---+
    | 1  | 2 | 3 | 4 | 5 |
    +----+---+---+---+---+
    | NG | o | o | o | o |
    | u  | o | o | o | o |
    +----+---+---+---+---+
      ng   ng  ng  ng  ng   = 5 max
     */
    val usedMachines: Map[Group, Int] = Map(NoGroup -> 1)
    val maxQuota = maximumQuota(NoGroup, usedMachines, machinePoolSize = 5)
    maxQuota.withinQuota(NoGroup.minimumQuota) must be (true)
    maxQuota must be (FiniteQuota(5))
  }

  "The minimum group quota" must "be guaranteed for all groups" in {
    /*
    +---+---+---+----+----+----+----+----+---+----+
    | 1 | 2 | 3 | 4  | 5  | 6  | 7  | 8  | 9 | 10 |
    +---+---+---+----+----+----+----+----+---+----+
    | A | A | A | B  | B  | B  | B  | NG | o | o  |
    | r | r | r | ur | ur | ur | ur | u  | o | o  |
    +---+---+---+----+----+----+----+----+---+----+
      a   a   a   b    b    b    b    ng   +1   +1    = { a->5max, b->6max, ng->3max }
     */
    val (a, b, ng) = (
      GuaranteedGroup("A", minimumQuota = FiniteQuota(3)),
      GuaranteedGroup("B", minimumQuota = FiniteQuota(4)),
      NoGroup
    )
    val usedMachines: Map[Group, Int] = Map(
      a -> 0,
      b -> 4,
      ng -> 1
    )
    val machinePoolSize = 10

    maximumQuota(a, usedMachines, machinePoolSize).withinQuota(a.minimumQuota) must be (true)
    maximumQuota(b, usedMachines, machinePoolSize).withinQuota(b.minimumQuota) must be (true)
    maximumQuota(ng, usedMachines, machinePoolSize).withinQuota(ng.minimumQuota) must be (true)

    maximumQuota(a, usedMachines, machinePoolSize) must be (FiniteQuota(5))
    maximumQuota(b, usedMachines, machinePoolSize) must be (FiniteQuota(6))
    maximumQuota(ng, usedMachines, machinePoolSize) must be (FiniteQuota(3))
  }

  "The occupied number of machines " must "be calculated individually for each group" in {
    /*
    In this scenario the total number of reserved machines is equal to the total number
    of used machines (5) but not all reserved machines are used.
    This forces to check individually each group for the maximum number of occupied machines
    which is max(group.reserved, group.used)
    +----+----+---+---+---+----+----+----+---+----+
    | 1  | 2  | 3 | 4 | 5 | 6  | 7  | 8  | 9 | 10 |
    +----+----+---+---+---+----+----+----+---+----+
    | A  | A  | A | B | B | NG | NG | NG | o | o  |
    | ur | ur | r | r | r | u  | u  | u  | o | o  |
    +----+----+---+---+---+----+----+----+---+----+
      a    a    a   b   b   ng   ng   ng   +1  +1   = { a->5max, b->4max, ng->5max }
     */
    val (a, b, ng) = (
      GuaranteedGroup("A", FiniteQuota(3)),
      GuaranteedGroup("B", FiniteQuota(2)),
      NoGroup
      )
    val usedMachines: Map[Group, Int] = Map(
      a -> 2,
      b -> 0,
      ng -> 3
    )
    val machinePoolSize = 10

    maximumQuota(a, usedMachines, machinePoolSize) must be (FiniteQuota(5))
    maximumQuota(b, usedMachines, machinePoolSize) must be (FiniteQuota(4))
    maximumQuota(ng, usedMachines, machinePoolSize) must be (FiniteQuota(5))
  }
}
