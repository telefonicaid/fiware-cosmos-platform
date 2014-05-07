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

package es.tid.cosmos.infinity.server.finatra

import com.twitter.finatra.Controller
import org.apache.hadoop.hdfs.server.datanode.DataNode

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.InfinityContentServerConfig
import es.tid.cosmos.infinity.server.urls.UrlMapper
import es.tid.cosmos.infinity.server.util.TwitterConversions._

class ContentRoutes(
    config: InfinityContentServerConfig,
    authService: AuthenticationService,
    dataNode: DataNode,
    urlMapper: UrlMapper) extends Controller {

  private val basePath = config.contentServerUrl.getPath
  private val actionValidator = new HttpContentActionValidator(config, dataNode)

  get(s"$basePath/*") { request =>
    val response = for {
      credentials <- HttpCredentialsValidator(request.remoteAddress, request)
      action <- actionValidator(request)
    } yield for {
        profile <- authService.authenticate(credentials)
        context = Action.Context(profile, urlMapper)
        result <- action(context)
      } yield ActionResultHttpRenderer(result)
    response.fold(error => ExceptionRenderer(error).toFuture, success => success.toTwitter)
  }

  error { request => request.error match {
    case Some(e) => ExceptionRenderer(e).toFuture
  }}
}
