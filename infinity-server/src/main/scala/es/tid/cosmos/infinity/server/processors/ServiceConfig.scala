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

package es.tid.cosmos.infinity.server.processors

import scala.collection.JavaConversions._

import akka.actor.ActorSystem
import com.typesafe.config.{Config, ConfigException}

case class ServiceConfig(
  name: String,
  webhdfsHostname: String,
  webhdfsPort: Int,
  infinityHostname: String,
  infinityPort: Int
)

object ServiceConfig {

  def apply(name: String)(implicit system: ActorSystem): Option[ServiceConfig] = try {
    val config = system.settings.config
    Some(ServiceConfig(
      name = name,
      webhdfsHostname = config.getString(s"infinity.server.services.$name.webhdfs.hostname"),
      webhdfsPort = config.getInt(s"infinity.server.services.$name.webhdfs.port"),
      infinityHostname = config.getString(s"infinity.server.services.$name.infinity.hostname"),
      infinityPort = config.getInt(s"infinity.server.services.$name.infinity.port")
    ))
  } catch {
    case e: ConfigException.Missing => None
  }

  def active(implicit system: ActorSystem): ServiceConfig = {
    val activeServiceProperty = "infinity.server.services.active"
    ServiceConfig(system.settings.config.getString(activeServiceProperty)).getOrElse(
      throw new IllegalArgumentException(
        s"missing $activeServiceProperty property in application config"))

  }

  def fromWebHdfs(hostname: String, port: Int)(implicit system: ActorSystem): Option[ServiceConfig] = {
    val config = system.settings.config
    val serviceConfig = config.getObject("infinity.server.services").withoutKey("active")
    val services = serviceConfig.toMap.map { case (srv, _) =>
      (srv,
        config.getString(s"infinity.server.services.$srv.webhdfs.hostname"),
        config.getInt(s"infinity.server.services.$srv.webhdfs.port"))
    }.toSeq
    val service = services.find {
      case (_, `hostname`, `port`) => true
      case _ => false
    }
    service.flatMap { case (serviceName, _, _) => ServiceConfig(serviceName) }
  }
}
