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

import com.typesafe.config.Config
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.fs.sql.InfinityDataStoreSqlComponent

class MetadataServer(
    namenodeProtocols: NamenodeProtocols,
    override val config: Config,
    authService: AuthenticationService) extends ConfigComponent with InfinityDataStoreSqlComponent {

  val serverConfig = FinatraServerCfg(
    http = Some(s"0.0.0.0:${config.getString("metadata.port")}")
  )

  val server = new EmbeddableFinatraServer(serverConfig)

  server.register(new MetadataRoutes(authService))

  def start(): Unit = server.start()

  def shutdown(): Unit = server.stop()
}
