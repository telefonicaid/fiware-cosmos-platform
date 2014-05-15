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

import java.net.InetAddress
import javax.servlet.http.HttpServletResponse
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scalaz.Validation

import unfiltered.Async
import unfiltered.filter.async
import unfiltered.filter.async.Plan.Intent
import unfiltered.response._
import unfiltered.request.{Authorization, HttpRequest}

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory
import es.tid.cosmos.infinity.server.urls.UrlMapper
import es.tid.cosmos.infinity.server.authorization._
import es.tid.cosmos.infinity.server.errors.RequestParsingException

class ContentRoutes(
      config: ContentServerConfig,
      authService: AuthenticationService,
      clientFactory: DfsClientFactory,
      urlMapper: UrlMapper) extends async.Plan {

  import ContentRoutes._

  //TODO: Extract common code between content and metadata plugin server and routes
  private val actionValidator = new HttpContentActionValidator(config, clientFactory)
  private val renderResult = new ContentActionResultRenderer(config.chunkSize)

  override def intent: Intent = { case request =>
    val response = for {
      authInfo <- authorizationInfo(request.remoteAddr, request)
      credentials <- HttpCredentialsValidator(authInfo)
      action <- actionValidator(request)
    } yield for {
        profile <- authService.authenticate(credentials)
        context = Action.Context(profile, urlMapper)
        result <- action(context)
      } yield renderResult(result)
    val responder = Responder(request)
    response.fold(
      error => responder.respond(ExceptionRenderer(error)),
      success => responder.respond(success)
    )
  }
}

private object ContentRoutes {
  lazy val ExceptionRenderer = new UnfilteredExceptionRenderer[HttpServletResponse]

  private def authorizationInfo[T](
      from: String, request: HttpRequest[T]): Validation[RequestParsingException, AuthInfo] = {
    import scalaz.Scalaz._
    request match {
      case Authorization(header) => AuthInfo(InetAddress.getByName(from), header).success
      case _ => RequestParsingException.MissingAuthorizationHeader().failure
    }
  }

  case class Responder[T](responder: Async.Responder[T]) {
    def respond(response_> : Future[ResponseFunction[T]]): Unit = {
      response_>.onSuccess { case response => responder.respond(response) }
      response_>.onFailure { case e => responder.respond(InternalServerError) } //TODO: Log error here
    }

    def respond(response: ResponseFunction[T]): Unit = {
      responder.respond(response)
    }
  }
}
