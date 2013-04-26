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

package es.tid.cosmos.api.controllers

import play.api.libs.json._
import play.api.libs.functional.syntax._
import es.tid.cosmos.servicemanager.ClusterId
import play.api.mvc.RequestHeader
import es.tid.cosmos.api.controllers.cluster.ClusterResource

/**
 * @author sortega
 */
package object clusters {

  case class CreateClusterParams(name: String, size: Int)

  implicit val createClusterParamsReads: Reads[CreateClusterParams] = (
    (__ \ "name").read[String] ~
    (__ \ "size").read[Int]
  )(CreateClusterParams.apply _)

  implicit object CreateClusterParamsWrites extends Writes[CreateClusterParams] {
    def writes(params: CreateClusterParams) = Json.obj(
      "name" -> JsString(params.name),
      "size" -> JsNumber(params.size)
    )
  }

  case class ClusterReference(id: String, href: String)
  object ClusterReference {
    def apply(clusterId: ClusterId)(implicit request: RequestHeader): ClusterReference =
      ClusterReference(clusterId.toString, ClusterResource.clusterUrl(clusterId))
  }
  case class ClusterList(clusterReferences: Seq[ClusterReference])

  implicit object ClusterReferenceWrites extends Writes[ClusterReference] {
    def writes(ref: ClusterReference): JsValue = Json.obj(
      "id" -> ref.id,
      "href" -> ref.href
    )
  }

  implicit object ClusterListWrites extends Writes[ClusterList] {
    def writes(list: ClusterList): JsValue = Json.obj(
      "clusters" -> JsArray(list.clusterReferences.map(ref => Json.toJson(ref)))
    )
  }
}
