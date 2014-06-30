/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

  it must "provide the quota of a consumer" in {
    val quotas = GlobalGroupQuotas(Map(
      GuaranteedGroup("GroupA", Quota(10)) -> Set("user")
    ))
    quotas.get("GroupA") must be (Quota(10))
    quotas.get(NoGroup.name) must be (EmptyQuota)
  }

  it must "provide the group of a consumer" in {
    val groupA = GuaranteedGroup("A", Quota(10))
    val quotas = GlobalGroupQuotas(Map(
      groupA -> Set("user")
    ))
    quotas.groupOf("user") must be (groupA)
    quotas.groupOf("other") must be (NoGroup)
  }
}
