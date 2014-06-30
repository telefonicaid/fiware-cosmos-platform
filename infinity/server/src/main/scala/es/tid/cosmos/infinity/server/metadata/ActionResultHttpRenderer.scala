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

package es.tid.cosmos.infinity.server.metadata

import org.apache.commons.logging.LogFactory
import unfiltered.request.HttpRequest
import unfiltered.response.{Created => _, _}

import es.tid.cosmos.infinity.common.json.MetadataFormatter
import es.tid.cosmos.infinity.server.actions.MetadataAction._

/** An object able to render action results into HTTP responses. */
object ActionResultHttpRenderer {

  private val metadataFormatter = new MetadataFormatter
  private val log = LogFactory.getLog(this.getClass)

  def apply[Response](request: HttpRequest[_], result: Result): ResponseFunction[Response] =
    result match {
      case Retrieved(metadata) =>
        renderJson(request, Ok, metadataFormatter.format(metadata))
      case Created(metadata) =>
        renderJson(request, unfiltered.response.Created, metadataFormatter.format(metadata))
      case Moved(metadata) =>
        renderJson(request, unfiltered.response.Created, metadataFormatter.format(metadata))
      case Deleted(_) | OwnerSet(_) | GroupSet(_) | PermissionsSet(_) =>
        renderPlain(request, NoContent)
      case _ =>
        renderPlain(
          request, InternalServerError, "No HTTP rendering method defined for action result")
    }

  private def renderJson[Response](
      request: HttpRequest[_], status: Status, content: String): ResponseFunction[Response] = {
    log.debug(s"${requestLine(request)} => $status $content")
    status ~> JsonContent ~> ResponseString(content)
  }

  private def renderPlain[Response](
      request: HttpRequest[_],
      status: Status,
      content: Option[String] = None): ResponseFunction[Response] = {
    log.debug(s"${requestLine(request)} => $status $content")
    content.fold(status: ResponseFunction[Response]) { c => status ~> ResponseString(c) }
  }

  private def renderPlain[Response](
      request: HttpRequest[_], status: Status, content: String): ResponseFunction[Response] =
    renderPlain(request, status, Some(content))

  private def requestLine(request: HttpRequest[_]) =
    s"${request.method} ${request.uri} (auth: ${authentication(request)})"

  private def authentication(request: HttpRequest[_]): String =
    request.headers("Authentication").mkString(",")
}
