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

import unfiltered.jetty.Http

import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.{HdfsDataNode, DataNode, DfsClientFactory}
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

/** Unfiltered-based content server.
  *
  * @param clientFactory the DFSClient factory for accessing the underlying file system
  * @param config        the server configuration
  * @param authService   the authentication service
  */
class ContentServer(
    clientFactory: DfsClientFactory,
    config: ContentServerConfig,
    authService: AuthenticationService) {

  private lazy val server = Http(config.localContentServerUrl.getPort).plan(contentRoutes)

  private val urlMapper = new InfinityUrlMapper(config)

  private val dataNode = new HdfsDataNode(clientFactory, config.bufferSize)

  private lazy val contentRoutes =
    new ContentRoutes(config, authService, dataNode, urlMapper)

  def start(): Unit = server.start()

  def stop(): Unit = server.stop()
}
