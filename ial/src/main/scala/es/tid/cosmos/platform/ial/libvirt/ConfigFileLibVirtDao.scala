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
