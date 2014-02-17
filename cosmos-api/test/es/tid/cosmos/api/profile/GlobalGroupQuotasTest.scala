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

import es.tid.cosmos.api.quota._

class GlobalGroupQuotasTest extends FlatSpec with MustMatchers {

  "Groups quotas" must "be invalid when groups are repeated" in {
    evaluating {
      GlobalGroupQuotas(Map(
        GuaranteedGroup("A", Quota(10)) -> Set(0),
        GuaranteedGroup("A", Quota(5)) -> Set(0)
      ))
    } must produce [IllegalArgumentException]
  }

  it must "provide reserved machines by group" in {
    val groupA = GuaranteedGroup("A", Quota(3))
    val groupB = GuaranteedGroup("B", Quota(2))
    val quotas = GlobalGroupQuotas(Map(
      groupA -> Set(0),
      groupB -> Set(1, 2)
    ))
    quotas.reservedUnusedResources(Map(
      0 -> 2,
      1 -> 1,
      2 -> 1,
      3 -> 3
    )) must equal (Map(
      groupA -> 1,
      groupB -> 0
    ))
  }
}
