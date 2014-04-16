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
