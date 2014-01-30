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

package es.tid.cosmos.api.controllers.info

import play.api.libs.json.{JsString, JsValue, Json, Writes}

import es.tid.cosmos.servicemanager.clusters.ClusterId

case class ResourcesInfo(
  groupConsumption: Int,
  individualConsumption: Int,
  available: Int,
  availableForGroup: Int,
  availableForUser: Int
)

object ResourcesInfo {
  implicit val resourcesInfoWrites = Json.writes[ResourcesInfo]
}

case class ClustersInfo(owned: Seq[ClusterId], accessible: Seq[ClusterId])

object ClustersInfo {

  implicit object ClusterIdWrites extends Writes[ClusterId] {
    override def writes(clusterId: ClusterId) = JsString(clusterId.id)
  }

  implicit val clustersInfoWrites = Json.writes[ClustersInfo]
}

case class GroupInfo(
  name: String,
  guaranteedQuota: Option[Int]
)

object GroupInfo {
  implicit object GroupInfoWrites extends Writes[GroupInfo] {
    override def writes(groupInfo: GroupInfo) =
      Json.obj("name" -> groupInfo.name) ++ quotaField(groupInfo.guaranteedQuota)

    private def quotaField(quotaOption: Option[Int]) = (
      for (quota <- quotaOption) yield Json.obj("guaranteedQuota" -> quota)
    ).getOrElse(Json.obj())
  }
}

/** General-purpose information about a platform user. */
case class Info(
  profileId: Long,
  handle: String,
  individualQuota: Option[Int],
  group: GroupInfo,
  clusters: ClustersInfo,
  resources: ResourcesInfo
)

object Info {
  implicit val infoWrites = Json.writes[Info]
}
