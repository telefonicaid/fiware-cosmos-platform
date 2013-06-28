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

import es.tid.cosmos.api.controllers.cluster.ClusterResource
import es.tid.cosmos.servicemanager.ClusterDescription

case class ClusterReference(
    id: String,
    href: String,
    name: String,
    state: String,
    stateDescription: String)

object ClusterReference {

  def apply(cluster: ClusterDescription)(implicit request: RequestHeader): ClusterReference =
    ClusterReference(
      id = cluster.id.toString,
      href = ClusterResource.clusterUrl(cluster.id),
      name = cluster.name,
      state = cluster.state.name,
      stateDescription = cluster.state.descLine)

  implicit object ClusterReferenceWrites extends Writes[ClusterReference] {
    def writes(ref: ClusterReference): JsValue = Json.obj(
      "id" -> ref.id,
      "href" -> ref.href,
      "name" -> ref.name,
      "state" -> ref.state,
      "stateDescription" -> ref.stateDescription
    )
  }
}

