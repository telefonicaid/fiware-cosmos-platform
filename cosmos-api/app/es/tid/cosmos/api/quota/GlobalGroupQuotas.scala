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
  def reservedUnusedResources(usageByConsumer: Map[ConsumerId, Int]): Map[GuaranteedGroup, Int] = {

    def groupUsage(members: Set[ConsumerId]): Int = (for {
      member <- members.toSeq
      resources <- usageByConsumer.get(member)
    } yield resources).sum

    for {
      (group, members) <- membersByGroup
      groupResources = group.minimumQuota.toInt.getOrElse(0)
    } yield group -> max(0, groupResources - groupUsage(members))
  }

  def get(name: String): Option[GuaranteedGroup] = membersByGroup.keys.collectFirst {
    case group if group.name == name => group
  }
}
