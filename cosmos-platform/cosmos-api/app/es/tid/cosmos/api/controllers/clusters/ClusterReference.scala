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

import play.api.libs.json.{Json, JsValue, Writes}
import play.api.mvc.RequestHeader

import es.tid.cosmos.servicemanager.ClusterId
import es.tid.cosmos.api.controllers.cluster.ClusterResource

/**
 * @author sortega
 */
case class ClusterReference(id: String, href: String)

object ClusterReference {
  def apply(clusterId: ClusterId)(implicit request: RequestHeader): ClusterReference =
    ClusterReference(clusterId.toString, ClusterResource.clusterUrl(clusterId))

  implicit object ClusterReferenceWrites extends Writes[ClusterReference] {
    def writes(ref: ClusterReference): JsValue = Json.obj(
      "id" -> ref.id,
      "href" -> ref.href
    )
  }
}

