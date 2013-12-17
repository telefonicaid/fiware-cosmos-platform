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

package es.tid.cosmos.platform.ial.libvirt

import scala.collection.JavaConversions

import com.typesafe.config.{ConfigFactory, Config}

import es.tid.cosmos.platform.ial.MachineProfile

class ConfigFileLibVirtDao(config: Config) extends LibVirtDao {

  import ConfigFileLibVirtDao._

  def this() { this(ConfigFactory.load()) }

  private val serverConfig: Seq[Config] = JavaConversions.asScalaBuffer(
    config.getConfigList("servers")
  )

  def libVirtServers: Seq[LibVirtServerProperties] = serverConfig.filter(
    getOptionalBoolean("enabled")
  ).map(ServerProperties.apply)
}

object ConfigFileLibVirtDao {

  private[libvirt] case class ServerProperties(config: Config) extends LibVirtServerProperties {
    val name: String = config.getString("name")
    val description: String = getStringWithFallback("description", "name")(config)
    val profile: MachineProfile.Value = MachineProfile.withName(config.getString("profile"))
    val rack: String = config.getString("rack")
    val connectionChain: String = config.getString("connection")
    val domainHostname: String = config.getString("domain_hostname")
    val domainIpAddress: String = config.getString("domain_ipaddress")
  }

  private def getOptionalBoolean(key: String)(conf: Config) =
    !conf.hasPath(key) || conf.getBoolean(key)

  private def getStringWithFallback(primaryKey: String, secondaryKey: String)(conf: Config) =
    if (conf.hasPath(primaryKey)) conf.getString(primaryKey) else conf.getString(secondaryKey)
}
