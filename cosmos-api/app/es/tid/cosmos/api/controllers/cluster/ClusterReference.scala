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

package es.tid.cosmos.api.controllers.cluster

import org.apache.commons.lang.time.DateFormatUtils
import play.api.libs.json.{Json, JsValue, Writes}
import play.api.mvc.RequestHeader

import es.tid.cosmos.api.profile.Cluster
import es.tid.cosmos.servicemanager.clusters.ClusterDescription

case class ClusterReference(description: ClusterDescription, assignment: Cluster) {
  require(description.id == assignment.clusterId)

  def withAbsoluteUri(request: RequestHeader): AbsoluteUriClusterReference =
    AbsoluteUriClusterReference(ClusterResource.clusterUrl(description.id)(request), this)
}

case class AbsoluteUriClusterReference(href: String, clusterReference: ClusterReference)

object AbsoluteUriClusterReference {
  val timestampFormat = DateFormatUtils.ISO_DATETIME_TIME_ZONE_FORMAT

  implicit object JsonWrites extends Writes[AbsoluteUriClusterReference] {
    def writes(obj: AbsoluteUriClusterReference): JsValue =
      obj match {
        case AbsoluteUriClusterReference(href, ClusterReference(desc, assig)) => Json.obj(
          "id" -> desc.id.toString,
          "href" -> href,
          "name" -> desc.name.underlying,
          "state" -> desc.state.name,
          "stateDescription" -> desc.state.descLine,
          "creationDate" -> timestampFormat.format(assig.creationDate)
        )
      }
    }
}
