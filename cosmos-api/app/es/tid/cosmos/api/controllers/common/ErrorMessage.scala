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

import play.api.Play
import play.api.Play.current
import play.api.libs.json._

/**
 * Represents a failed action.
 *
 * @param error      Error description
 * @param exception  Optional associated exception
 */
case class ErrorMessage(error: String, exception: Option[Throwable] = None)

object ErrorMessage {
  def apply(error: String, exception: Throwable): ErrorMessage =
    ErrorMessage(error, Some(exception))

  implicit object ErrorMessageWrites extends Writes[ErrorMessage] {
    def writes(message: ErrorMessage): JsValue = {
      message.exception match {
        case Some(ex) if (!Play.isProd) => Json.obj(
          "error" -> s"${message.error}: ${ex.getMessage}",
          "exception" -> ex.getClass.getCanonicalName,
          "stack_trace" -> ex.getStackTraceString
        )
        case _ => Json.obj("error" -> message.error)
      }
    }
  }
}
