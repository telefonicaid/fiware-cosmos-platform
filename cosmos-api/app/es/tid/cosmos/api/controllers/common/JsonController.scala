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

package es.tid.cosmos.api.controllers.common

import scalaz._

import play.api.data.validation.ValidationError
import play.api.libs.json._
import play.api.mvc.{Controller, Request}

/** JSON consuming controller. */
trait JsonController { this: Controller =>

  def validJsonBody[Payload: Reads](request: Request[JsValue]): ActionValidation[Payload] = {
    import Scalaz._

    Json.fromJson[Payload](request.body).fold(
      invalid = errors => formatErrors(errors).failure,
      valid = payload => payload.success
    )
  }

  private def formatErrors(errorsByPath: Seq[(JsPath, Seq[ValidationError])]) = {
    val formattedErrors = errorsByPath.map {
      case (path, errors) => (path.toString(), errors.map(_.message))
    }
    BadRequest(Json.toJson(Map(formattedErrors: _*)))
  }
}
