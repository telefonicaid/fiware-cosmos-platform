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

import akka.actor.{ActorSystem, Props}
import akka.io.IO
import akka.pattern.ask
import akka.util.Timeout
import com.typesafe.config.ConfigFactory
import spray.can.Http

object Boot extends App {

  val config = ConfigFactory.load()

  implicit val system = ActorSystem("infinity-server")

  val service = system.actorOf(Props[InfinityActor], "infinity-server-service")

  implicit val timeout = Timeout(5.seconds)

  IO(Http) ? Http.Bind(service,
    interface = config.getString("cosmos.infinity.server.interface"),
    port = config.getInt("cosmos.infinity.server.port"))
}
