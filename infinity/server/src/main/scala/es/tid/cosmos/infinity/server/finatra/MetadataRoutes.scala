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
package es.tid.cosmos.infinity.server.finatra

import scala.concurrent.ExecutionContext.Implicits.global

import com.twitter.finatra.{Controller, Request}
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.util.TwitterConversions._

class MetadataRoutes(
    authService: AuthenticationService,
    nameNode: NamenodeProtocols) extends Controller {

  private val contentPath = "/infinityfs/v1/metadata/*"
  private val actionValidator = new HttpActionValidator(nameNode)

  get(contentPath) { request =>
    val path = getPath(request)
    val response = for {
      credentials <- HttpCredentialsValidator(request.remoteAddress, request)
      action <- actionValidator(request)
    } yield for {
      profile <- authService.authenticate(credentials)
    } yield render.plain(s"metadata of $path")
    response.fold(error => ExceptionRenderer(error).toFuture, success => success.toTwitter)
  }

  error { request => request.error match {
    case Some(e) => ExceptionRenderer(e).toFuture
  }}

  // `splat` is an undocumented Finatra tag. For wilcards routes extractions see
  // https://github.com/twitter/finatra/blob/master/src/main/scala/com/twitter/finatra/PathParser.scala
  private def getPath(request: Request): String = "/" + request.routeParams.getOrElse("splat", "")
}
