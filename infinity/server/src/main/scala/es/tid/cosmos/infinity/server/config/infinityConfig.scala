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

package es.tid.cosmos.infinity.server.config

import java.net.{URI, InetAddress, URL}

import com.typesafe.config.{Config, ConfigException}

import es.tid.cosmos.infinity.common.util.UriUtil
import es.tid.cosmos.infinity.server.config.InfinityConfig._

abstract class InfinityConfig(config: Config) {

  val metadataProtocol = config.getString("metadata.protocol")
  val metadataHost = config.getString("metadata.host")
  val metadataPort = config.getInt("metadata.port")
  val metadataBasePath = mapOpt(config.getString("metadata.basePath")).getOrElse(DefaultBasePath)
  val metadataBaseUrl = new URL(
      s"$metadataProtocol://$metadataHost:$metadataPort/$metadataBasePath")

  def contentServerUrl(hostname: String): Option[URL] = {
    val protocol = mapOpt(config.getString(s"contentServer.$hostname.protocol"))
      .getOrElse(DefaultProtocol)
    val port = mapOpt(config.getInt(s"contentServer.$hostname.port"))
    val basePath = mapOpt(config.getString(s"contentServer.$hostname.basePath"))
      .getOrElse(DefaultBasePath)
    port.map(new URL(protocol, hostname, _, basePath))
  }
}

class MetadataServerConfig(config: Config) extends InfinityConfig(config) {
  val replication = mapOpt(config.getInt("metadata.replication").toShort).getOrElse(DefaultReplication)
  val blockSize = mapOpt(config.getLong("metadata.blockSize")).getOrElse(DefaultBlockSize)
}

class ContentServerConfig(config: Config) extends InfinityConfig(config) {
  val chunkSize = mapOpt(config.getInt("infinity.server.content.chunkSize"))
    .getOrElse(DefaultChunkSize)

  val nameNodeRPCUrl: URL = UriUtil.replaceScheme(
    new URI(config.getString(NameNodeHdfsAddressKey)), "http").toURL

  val localContentServerUrl: URL = {
    val hostname = InetAddress.getLocalHost.getHostName
    contentServerUrl(hostname).getOrElse(throw new IllegalArgumentException(
      s"Cannot initialize server because contentServer.$hostname configuration is missing."))
  }
}

object InfinityConfig {
  val DefaultProtocol: String = "https"
  val DefaultBasePath: String = "/infinityfs/v1"
  val DefaultReplication: Short = 3
  val DefaultBlockSize: Long = 64l * 1024l * 1024l
  val NameNodeHdfsAddressKey = "fs.defaultFS"
  // 4096 is taken from Hadoop IOUtils.copy
  val DefaultChunkSize = 4096
  val HadoopKeys = Seq(NameNodeHdfsAddressKey)

  def mapOpt[T](f: => T): Option[T] = try { Some(f) } catch {
    case _: ConfigException.Missing => None
  }
}
