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

import es.tid.cosmos.infinity.server.config.InfinityConfig._
import es.tid.cosmos.infinity.server.plugin.PluginConfig

trait InfinityConfig {

  /** Either http or https */
  def metadataProtocol: String

  /** Hostname of the metadata server.
    *
    * @throws com.typesafe.config.ConfigException$Missing  As there is no default value for it
    */
  def metadataHost: String

  def metadataPort: Int

  def metadataBasePath: String

  def metadataBaseUrl: URL

  /** Builds the content server url from just the hostname. */
  def contentServerUrl(hostname: String): URL
}

abstract class BaseInfinityConfig(config: Config) extends InfinityConfig {

  override val metadataProtocol =
    withDefault(config.getString("metadata.protocol"), DefaultProtocol)
  override val metadataHost = config.getString("metadata.host")
  override val metadataPort = withDefault(config.getInt("metadata.port"), DefaultMetadataPort)
  override val metadataBasePath =
    withDefault(config.getString("metadata.basePath"), DefaultMetadataBasePath)
  override val metadataBaseUrl =
    new URL(metadataProtocol, metadataHost, metadataPort, metadataBasePath.toString)

  override def contentServerUrl(hostname: String): URL = {
    val protocol =
      withDefault(config.getString(s"content.server.$hostname.protocol"), DefaultProtocol)
    val port = withDefault(config.getInt(s"content.server.$hostname.port"), DefaultContentPort)
    val basePath =
      withDefault(config.getString(s"content.server.$hostname.basePath"), DefaultContentBasePath)
    new URL(protocol, hostname, port, basePath.toString)
  }

  protected def withDefault[T](block: => T, default: T): T = try {
    block
  } catch {
    case _: ConfigException.Missing => default
  }
}

class MetadataServerConfig(config: Config) extends BaseInfinityConfig(config) {
  val replication = withDefault(config.getInt("metadata.replication").toShort, DefaultReplication)
  val blockSize = withDefault(config.getLong("metadata.blockSize"), DefaultBlockSize)
}

class ContentServerConfig(config: Config) extends BaseInfinityConfig(config) {
  val chunkSize = withDefault(config.getInt("content.chunkSize"), DefaultChunkSize)
  val bufferSize = withDefault(config.getInt("content.bufferSize"), DefaultBufferSize)

  val nameNodeRPCUri: URI =
    new URI(config.getString(s"${PluginConfig.HadoopKeyPrefix}.$NameNodeHdfsAddressKey"))

  val localContentServerUrl: URL = contentServerUrl(InetAddress.getLocalHost.getHostName)
}

object InfinityConfig {
  val DefaultProtocol: String = "http"
  val DefaultContentPort: Int = 51075
  val DefaultMetadataPort: Int = 51070
  val DefaultMetadataBasePath: String = "/infinityfs/v1/metadata"
  val DefaultContentBasePath: String = "/infinityfs/v1/content"
  val DefaultReplication: Short = 3
  val DefaultBlockSize: Long = 64l * 1024l * 1024l
  val NameNodeHdfsAddressKey = "fs.defaultFS"
  // 4096 is taken from Hadoop IOUtils.copy
  val DefaultChunkSize, DefaultBufferSize = 4096
  val HadoopKeys = Seq(NameNodeHdfsAddressKey)
}
