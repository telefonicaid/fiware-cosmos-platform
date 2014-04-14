/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
package es.tid.cosmos.infinity.server

import com.typesafe.config.Config
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.infinity.server.actions.MetadataActions
import es.tid.cosmos.infinity.server.finatra.{EmbeddableFinatraServer, FinatraServerCfg}
import es.tid.cosmos.infinity.server.fs.sql.InfinityDataStoreSqlComponent

class InfinityMetadataServer(namenodeProtocols: NamenodeProtocols, override val config: Config)
  extends ConfigComponent
  with InfinityDataStoreSqlComponent {

  val serverConfig = FinatraServerCfg(
    http = Some(config.getString("cosmos.infinity.server.http"))
  )

  val server = new EmbeddableFinatraServer(serverConfig)

  server.register(new MetadataActions)

  def start(): Unit = server.start()

  def shutdown(): Unit = server.stop()

}
