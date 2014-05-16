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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.Try

import unfiltered.filter.async
import unfiltered.filter.async.Plan.Intent

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.authorization.{AuthInfo, HttpCredentialsValidator}
import es.tid.cosmos.infinity.server.config.InfinityConfig
import es.tid.cosmos.infinity.server.hadoop.NameNode
import es.tid.cosmos.infinity.server.unfiltered.response.Responder
import es.tid.cosmos.infinity.server.urls.UrlMapper

class MetadataRoutes(
    config: InfinityConfig,
    authService: AuthenticationService,
    nameNode: NameNode,
    urlMapper: UrlMapper) extends async.Plan {

  private val actionValidator = new HttpActionValidator(config, nameNode)

  override def intent: Intent = { case request =>
    val response = for {
      authInfo <- AuthInfo(request.remoteAddr, request)
      credentials <- HttpCredentialsValidator(authInfo)
      action <- actionValidator(request)
    } yield for {
        profile <- authService.authenticate(credentials)
        context = Action.Context(profile, urlMapper)
        result <- action(context)
      } yield ActionResultHttpRenderer(result)
    response.transform(
      success_> => Try(Responder.respond(request, success_>)),
      failure => Try(Responder.respond(request, failure))
    )
  }
}
