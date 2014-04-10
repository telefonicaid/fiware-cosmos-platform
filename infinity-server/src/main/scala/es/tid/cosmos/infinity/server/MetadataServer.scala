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

import scala.concurrent.duration._

import akka.actor.ActorSystem
import akka.io.IO
import akka.pattern.ask
import akka.util.Timeout
import org.apache.hadoop.hdfs.server.namenode.NameNode
import spray.can.Http

import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.infinity.server.app.InfinityAppComponent
import es.tid.cosmos.infinity.server.authentication.cosmosapi.CosmosApiAuthenticationComponent
import es.tid.cosmos.infinity.server.fs.sql.InfinityDataStoreSqlComponent
import es.tid.cosmos.infinity.server.processors.DefaultRequestProcessorComponent
import com.typesafe.config.Config

class MetadataServer(nameNode: NameNode, override val config: Config)
  extends InfinityAppComponent
  with CosmosApiAuthenticationComponent
  with InfinityDataStoreSqlComponent
  with DefaultRequestProcessorComponent
  with ConfigComponent {

  implicit val system = ActorSystem("infinity-server", config)

  val service = system.actorOf(infinityAppProps)

  implicit val timeout = Timeout(5.seconds)


  def start(): Unit = IO(Http) ? Http.Bind(
    listener = service,
    interface = config.getString("cosmos.infinity.server.interface"),
    port = config.getInt("cosmos.infinity.server.port")
  )

  def shutdown(): Unit = {
    system.shutdown()
    system.awaitTermination()
  }
}
