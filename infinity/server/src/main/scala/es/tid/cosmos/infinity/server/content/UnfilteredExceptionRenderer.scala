/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.content

import org.apache.commons.logging.LogFactory
import unfiltered.request.HttpRequest
import unfiltered.response._

import es.tid.cosmos.infinity.server.errors.ExceptionRenderer

class UnfilteredExceptionRenderer[Request <: HttpRequest[_], Response]
    extends ExceptionRenderer[Request, ResponseFunction[Response]] {

  val log = LogFactory.getLog(classOf[UnfilteredExceptionRenderer[Request, Response]])

  override protected def withAuthHeader(
      response: ResponseFunction[Response], headerContent: String): ResponseFunction[Response] =
    response ~> WWWAuthenticate(headerContent)

  override protected def render(
      request: Request, status: Int, jsonContent: String): ResponseFunction[Response] = {
    log.warn(s"${requestLine(request)} => $status $jsonContent")
    Status(status) ~> JsonContent ~> ResponseString(jsonContent)
  }

  private def requestLine(request: Request) =
    s"${request.method} ${request.uri} (auth: ${authentication(request)})"

  private def authentication(request: Request): String =
    request.headers("Authentication").mkString(",")
}
