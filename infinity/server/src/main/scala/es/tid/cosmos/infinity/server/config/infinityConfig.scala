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

  val metadataProtocol = withDefault(config.getString("metadata.protocol"), DefaultProtocol)
  val metadataHost = config.getString("metadata.host")
  val metadataPort = withDefault(config.getInt("metadata.port"), DefaultMetadataPort)
  val metadataBasePath = withDefault(config.getString("metadata.basePath"), DefaultBasePath)
  val metadataBaseUrl = new URL(
      s"$metadataProtocol://$metadataHost:$metadataPort/$metadataBasePath/metadata")

  def contentServerUrl(hostname: String): URL = {
    val protocol = withDefault(config.getString(s"content.server.$hostname.protocol"), DefaultProtocol)
    val port = withDefault(config.getInt(s"content.server.$hostname.port"), DefaultContentPort)
    val basePath = withDefault(config.getString(s"content.server.$hostname.basePath"), DefaultBasePath)
    new URL(protocol, hostname, port, s"$basePath/content")
  }

  protected def withDefault[T](block: => T, default: T): T = try {
    block
  } catch {
    case _: ConfigException.Missing => default
  }
}

class MetadataServerConfig(config: Config) extends InfinityConfig(config) {
  val replication = withDefault(config.getInt("metadata.replication").toShort, DefaultReplication)
  val blockSize = withDefault(config.getLong("metadata.blockSize"), DefaultBlockSize)
}

class ContentServerConfig(config: Config) extends InfinityConfig(config) {
  val chunkSize = withDefault(config.getInt("content.chunkSize"), DefaultChunkSize)

  val nameNodeRPCUrl: URL = UriUtil.replaceScheme(
    new URI(config.getString(NameNodeHdfsAddressKey)), "http").toURL

  val localContentServerUrl: URL = contentServerUrl(InetAddress.getLocalHost.getHostName)
}

object InfinityConfig {
  val DefaultProtocol: String = "https"
  val DefaultContentPort: Int = 50175
  val DefaultMetadataPort: Int = 50170
  val DefaultBasePath: String = "/infinityfs/v1"
  val DefaultReplication: Short = 3
  val DefaultBlockSize: Long = 64l * 1024l * 1024l
  val NameNodeHdfsAddressKey = "fs.defaultFS"
  // 4096 is taken from Hadoop IOUtils.copy
  val DefaultChunkSize = 4096
  val HadoopKeys = Seq(NameNodeHdfsAddressKey)
}
