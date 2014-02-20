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

import play.api.data.validation.ValidationError
import play.api.libs.functional.syntax._
import play.api.libs.json._

import es.tid.cosmos.servicemanager.ClusterName

/** Parameters for cluster creation. */
case class CreateClusterParams(name: ClusterName, size: Int, optionalServices: Seq[String])

object CreateClusterParams {
  implicit val createClusterParamsReads: Reads[CreateClusterParams] = (
    (__ \ "name").read[String]
      .filter(ValidationError("cluster name is too long"))(_.size <= ClusterName.MaxLength)
      .map(ClusterName.apply) ~
    (__ \ "size").read[Int] ~
    (__ \ "optionalServices").readNullable[Seq[String]].map(_.getOrElse(Seq()))
  )(CreateClusterParams.apply _)

  implicit object CreateClusterParamsWrites extends Writes[CreateClusterParams] {
    def writes(params: CreateClusterParams) = Json.obj(
      "name" -> params.name.underlying,
      "size" -> params.size,
      "optionalServices" -> params.optionalServices
    )
  }
}
