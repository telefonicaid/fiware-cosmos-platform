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

package es.tid.cosmos.infinity.server.finagle

import scala.concurrent.ExecutionContext.Implicits.global

import com.twitter.finagle.Service
import com.twitter.finagle.http.Request
import com.twitter.finagle.stream.StreamResponse
import com.twitter.util.{Future => TwitterFuture}

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.finagle.StreamConversions._
import es.tid.cosmos.infinity.server.finatra._
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory
import es.tid.cosmos.infinity.server.util.TwitterConversions._
import es.tid.cosmos.infinity.server.urls.UrlMapper

class ContentStreamRoutes(
      config: ContentServerConfig,
      authService: AuthenticationService,
      clientFactory: DfsClientFactory,
      urlMapper: UrlMapper) extends Service[Request, StreamResponse] {
  //TODO: Extract common code between content and metadata plugin server and routes
  private val actionValidator = new HttpContentActionValidator(config, clientFactory)
  private val renderResult = new ActionResultStreamRenderer(config.chunkSize)

  override def apply(request: Request): TwitterFuture[StreamResponse] = {
    val response = for {
      credentials <- HttpCredentialsValidator(request.remoteAddress, request)
      action <- actionValidator(request)
    } yield for {
        profile <- authService.authenticate(credentials)
        context = Action.Context(profile, urlMapper)
        result <- action(context)
      } yield renderResult(result)
    response.fold(
      error => ExceptionRenderer(error).toFuture.map(_.toStream),
      success => success.toTwitter
    )
  }
}
