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

import play.api.data.validation.ValidationError
import play.api.libs.functional.syntax._
import play.api.libs.json._

import es.tid.cosmos.servicemanager.ClusterName

/** Parameters for cluster creation. */
case class CreateClusterParams(
  name: ClusterName, size: Int, optionalServices: Seq[String], shared: Boolean)

object CreateClusterParams {
  implicit val createClusterParamsReads: Reads[CreateClusterParams] = (
    (__ \ "name").read[String]
      .filter(ValidationError("cluster name is too long"))(_.size <= ClusterName.MaxLength)
      .map(ClusterName.apply) ~
    (__ \ "size").read[Int] ~
    readNullableOrElse(__ \ "optionalServices", Seq.empty[String]) ~
    readNullableOrElse(__ \ "shared", true)
  )(CreateClusterParams.apply _)

  implicit object CreateClusterParamsWrites extends Writes[CreateClusterParams] {
    def writes(params: CreateClusterParams) = Json.obj(
      "name" -> params.name.underlying,
      "size" -> params.size,
      "optionalServices" -> params.optionalServices,
      "shared" -> params.shared
    )
  }

  private def readNullableOrElse[T: Reads](path: JsPath, defaultValue: T): Reads[T] =
    path.readNullable[T].map(_.getOrElse(defaultValue))
}
