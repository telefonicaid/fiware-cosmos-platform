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

package es.tid.cosmos.api.controllers.clusters

import org.apache.commons.lang.time.DateFormatUtils
import play.api.libs.json.{Writes, Json, JsValue}
import play.api.mvc.RequestHeader

import es.tid.cosmos.api.controllers.cluster.ClusterResource
import es.tid.cosmos.api.profile.ClusterAssignment
import es.tid.cosmos.servicemanager.ClusterDescription

case class ClusterReference(
    description: ClusterDescription,
    assignment: ClusterAssignment) {
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
          "name" -> desc.name,
          "state" -> desc.state.name,
          "stateDescription" -> desc.state.descLine,
          "creationDate" -> timestampFormat.format(assig.creationDate)
        )
      }
    }
}
