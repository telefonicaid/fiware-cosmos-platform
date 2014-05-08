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

import com.twitter.finagle.Service
import com.twitter.finagle.stream.StreamResponse
import org.jboss.netty.handler.codec.http.{HttpResponse, HttpMethod, HttpRequest}
import com.twitter.util.Future
import java.net.URI
import scala.util.matching.Regex
import es.tid.cosmos.infinity.server.actions.{Action, GetContent}
import java.io.{File, FileInputStream, InputStream}
import es.tid.cosmos.infinity.server.finatra.{ExceptionRenderer, ActionResultHttpRenderer, HttpCredentialsValidator}

/**
 * TODO: Insert description here
 *
 * @author adamos
 */
//class ContentStreamRoutes extends Service[HttpRequest, StreamResponse] {
//  override def apply(request: HttpRequest): Future[StreamResponse] = {
//    val path = new URI(request.getUri).getPath
//    val method = request.getMethod
//    (method, path) match {
//      case (HttpMethod.GET, basePath(subPath)) => getContent(subPath, responseHeader(request))
//    }
//  }
//
//  private def get() {
//    val response = for {
//      credentials <- HttpCredentialsValidator(request.remoteAddress, request)
//      action <- actionValidator(request)
//    } yield for {
//        profile <- authService.authenticate(credentials)
//        context = Action.Context(profile, urlMapper)
//        result <- action(context)
//      } yield ActionResultHttpRenderer(result)
//    response.fold(error => ExceptionRenderer(error).toFuture, success => success.toTwitter)
//  }
//
//  private def getContent(path: String, responseHeader: HttpResponse): StreamResponse = {
//    println("processing path " + path)
//    GetContent(null, null, null).apply()
//    val in: InputStream = new FileInputStream(new File(s"/Users/adamos/dev/workspaces/cosmos/cosmos-platform/infinity/server/src/main/scala/es/tid/cosmos/infinity/server/finagle/$path"))
//    new InputStreamResponse(in, responseHeader)
//  }
//}
