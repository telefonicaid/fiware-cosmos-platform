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

package es.tid.cosmos.api.controllers.common

import scalaz._

import play.api.data.validation.ValidationError
import play.api.libs.json._
import play.api.mvc.{Controller, Request}

/** JSON consuming controller. */
trait JsonController extends Controller {

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
