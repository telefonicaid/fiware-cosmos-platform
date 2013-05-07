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

import play.api.data.validation.ValidationError
import play.api.libs.json._
import play.api.mvc.{Action, Controller, Request, Result}

/**
 * JSON consuming controller.
 */
trait JsonController extends Controller {
  def JsonBodyAction[Payload: Reads](f: (Request[JsValue], Payload) => Result): Action[JsValue] =
    Action(parse.tolerantJson) { request =>
      Json.fromJson[Payload](request.body).fold(
        invalid = formatErrors,
        valid = payload => f(request, payload))
    }

  private def formatErrors(errors: Seq[(JsPath, Seq[ValidationError])]) = {
    val formattedErrors = errors.map {
      case (path, errors) => (path.toString(), errors.map(_.message))
    }
    BadRequest(Json.toJson(Map(formattedErrors: _*)))
  }
}
