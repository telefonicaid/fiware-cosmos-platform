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

package es.tid.cosmos.api.controllers.cluster

import com.wordnik.swagger.annotations.{ApiProperty, ApiClass}
import play.api.mvc.RequestHeader
import play.api.libs.json.{Json, JsValue, Writes}

import es.tid.cosmos.servicemanager.ClusterDescription

/**
 * A cluster from the perspective of API clients.
 */
case class ClusterDetails(
    href: String,
    id: String,
    name: String,
    size: Int,
    @ApiProperty(allowableValues = "provisioning,running,terminating,terminated,failed")
    state: String,
    stateDescription: String)

object ClusterDetails {
  /**
   * Create a ClusterDetails from a description in the context of a request.
   *
   * @param desc     Cluster description
   * @param request  Context request
   * @return         A ClusterDetails instance
   */
  def apply(desc: ClusterDescription)(implicit request: RequestHeader): ClusterDetails =
    ClusterDetails(
      href = ClusterResource.clusterUrl(desc.id),
      id = desc.id.toString,
      name = desc.name,
      size = desc.size,
      state = desc.state.name,
      stateDescription = desc.state.descLine
    )

  implicit object ClusterDetailsWrites extends Writes[ClusterDetails] {
    def writes(d: ClusterDetails): JsValue = Json.obj(
      "href" -> d.href,
      "id" -> d.id,
      "name" -> d.name,
      "size" -> d.size,
      "state" -> d.state,
      "state_description" -> d.stateDescription
    )
  }
}
