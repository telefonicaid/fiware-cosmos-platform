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

import play.api.libs.json._
import play.api.mvc.{Controller, Action, Result, Request}

/**
 * @author sortega
 */
trait JsonController extends Controller {
  def JsonBodyAction[Payload: Reads](f: (Request[JsValue], Payload) => Result): Action[JsValue] =
    Action(parse.tolerantJson) { request =>
      val parsedJson: JsResult[Payload] = Json.fromJson[Payload](request.body)
      parsedJson match {
        case JsSuccess(payload, _) => f(request, payload)
        case error@JsError(_) => BadRequest(formatJsError(error))
      }
    }

  private def formatJsError(jsError: JsError): JsValue = {
    val formattedErrors = jsError.errors.toList.map {
      case (path, errors) => (path.toString(), errors.map(_.message).toList)
    }
    Json.toJson(Map(formattedErrors: _*))
  }
}
