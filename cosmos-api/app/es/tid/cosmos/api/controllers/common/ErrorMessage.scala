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

import play.api.Play
import play.api.Play.current
import play.api.libs.json._

/** Represents a failed action.
  *
  * @param error      Error description
  * @param exception  Optional associated exception
  */
case class ErrorMessage(error: String, exception: Option[Throwable] = None)

object ErrorMessage {
  def apply(error: String, exception: Throwable): ErrorMessage =
    ErrorMessage(error, Some(exception))

  implicit object ErrorMessageWrites extends Writes[ErrorMessage] {
    def writes(message: ErrorMessage): JsValue =
      message.exception match {
        case Some(ex) if !Play.isProd => Json.obj(
          "error" -> s"${message.error}: ${ex.getMessage}",
          "exception" -> getExceptionName(ex),
          "stackTrace" -> getStackTraceLines(ex)
        )
        case _ => Json.obj("error" -> message.error)
      }

    private def getStackTraceLines(ex: Throwable) = JsArray(
      ex.getStackTrace.map(line => JsString(line.toString)))

    private def getExceptionName(ex: Throwable) = {
      val canonicalName = ex.getClass.getCanonicalName
      if (canonicalName != null) canonicalName else ex.getClass.getName
    }
  }
}
