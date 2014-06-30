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

package es.tid.cosmos.api.quota

import scala.math.max

/** System-wide quotas
  *
  * @constructor
  * @tparam ConsumerId     Type of the consumer identifier
  * @param membersByGroup  Group to members map
  */
case class GlobalGroupQuotas[ConsumerId](membersByGroup: Map[GuaranteedGroup, Set[ConsumerId]]) {

  require(membersByGroup.keySet.map(_.name).size == membersByGroup.size, "Repeated group names")

  /** Compute the available resources that should be reserved per group given the current
    * resource usage.
    *
    * @param usageByConsumer  Current resource usage
    * @return                Remaining reserved resources per group
    */
  def reservedUnusedResources(usageByConsumer: Map[ConsumerId, Int]): Map[Group, Int] = {

    def groupUsage(members: Set[ConsumerId]): Int = (for {
      member <- members.toSeq
      resources <- usageByConsumer.get(member)
    } yield resources).sum

    for {
      (group, members) <- membersByGroup
      groupResources = group.minimumQuota.toInt
    } yield group -> max(0, groupResources - groupUsage(members))
  }

  /** Determines the guaranteed quota given a group name */
  def get(name: String): LimitedQuota =
    membersByGroup.keys.find(_.name == name).map(_.minimumQuota).getOrElse(EmptyQuota)

  def groupOf(consumerId: ConsumerId): Group = membersByGroup.collectFirst {
    case (group, members) if members.contains(consumerId) => group
  }.getOrElse(NoGroup)
}
