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

import java.net.InetSocketAddress

import com.twitter.finagle.{Service, Filter}
import com.twitter.finagle.builder.{Server, ServerBuilder}
import com.twitter.finagle.http.{Request => FinagleRequest}
import com.twitter.finagle.stream.{Stream, StreamResponse}
import org.jboss.netty.handler.codec.http.HttpRequest

import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

/** Finagle-based content server that makes use of [[StreamResponse]] to allow serving content
  * in streams via chunks, something not currently supported with Finatra.
  *
  * @param clientFactory the DFSClient factory for accessing the underlying file system
  * @param config        the server configuration
  * @param authService   the authentication service
  */
class ContentStreamServer(
    clientFactory: DfsClientFactory,
    config: ContentServerConfig,
    authService: AuthenticationService) {

  private var server: Option[Server] = None

  private val urlMapper = new InfinityUrlMapper(config)

  private val nettyToFinagle =
    Filter.mk[HttpRequest, StreamResponse, FinagleRequest, StreamResponse] { (req, service) =>
      service(FinagleRequest(req))
    }

  private lazy val service: Service[HttpRequest, StreamResponse] = {
    val contentService = new ContentStreamRoutes(config, authService, clientFactory, urlMapper)
    nettyToFinagle andThen contentService
  }

  def start(): Unit = {
    server = Some(ServerBuilder()
      .codec(Stream())
      .bindTo(new InetSocketAddress(config.localContentServerUrl.getPort))
      .name("infinity_content_server")
      .build(service))
  }

  def stop(): Unit = {
    server.map(_.close())
  }
}
