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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

class InfinityServerIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(MasterNode -> "aMasterNodeName")

  val parameters = InfinityServerParameters(
    cosmosApiUrl = "http://some_host/endpoint",
    infinitySecret = "secret_token"
  )
  override def configurator = AmbariInfinityServer.configurator(parameters, resourcesConfigDirectory)

  "An InfinityServer service" must "have core and services configuration contributions" in {
    contributions.global must not be 'defined
    contributions.core must be ('defined)
    contributions.services.map(_.configType).toSet must be (Set("hdfs-site", "infinity-server"))
  }

  it must "configure the user-to-group-mapping" in {
    val core = contributions.core.get.properties
    val mapping = "hadoop.security.group.mapping"
    core.get(mapping) must be ('defined)
    core(s"$mapping.infinity.apiBase") must be (parameters.cosmosApiUrl)
    core(s"$mapping.infinity.timeout") must be (5000)
    core(s"$mapping.infinity.secret") must be (parameters.infinitySecret)
  }

  it must "configure Infinity plugins for HDFS" in {
    val config = contributions.services.find(_.configType == "hdfs-site").get.properties
    config.get("dfs.namenode.plugins") must be ('defined)
    config.get("dfs.datanode.plugins") must be ('defined)
    config.get("dfs.infinity.metadata.port") must be ('defined)
    config("dfs.infinity.metadata.host") must be (dynamicProperties(MasterNode))
  }

  it must "configure ports and certificates" in {
    val config = contributions.services.find(_.configType == "infinity-server").get.properties
    config.get("blocked_ports") must be ('defined)
    config.get("ssl_certificate_content") must be ('defined)
    config.get("ssl_certificate_key_content") must be ('defined)
  }
}
