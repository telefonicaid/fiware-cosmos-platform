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

import java.net.URL
import scala.collection.JavaConverters._

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class MetadataServerConfigTest extends FlatSpec with MustMatchers {

  it must "retrieve content server URL with all settings are present" in {
    val config = configFor(
      "content.server.content01.example.com.protocol" -> "https",
      "content.server.content01.example.com.port" -> "1234",
      "content.server.content01.example.com.basePath" -> "/inf/v1/content")
    config.contentServerUrl("content01.example.com") must be (new URL(
      "https://content01.example.com:1234/inf/v1/content"))
  }

  it must "retrieve content server URL when protocol is missing" in {
    val config = configFor(
      "content.server.content01.example.com.port" -> "1234",
      "content.server.content01.example.com.basePath" -> "/inf/v1/content")
    config.contentServerUrl("content01.example.com") must be (new URL(
      "https://content01.example.com:1234/inf/v1/content"))
  }

  it must "retrieve content server URL when base path is missing" in {
    val config = configFor("content.server.content01.example.com.port" -> "1234")
    config.contentServerUrl("content01.example.com") must be (new URL(
      "https://content01.example.com:1234/infinityfs/v1/content"))
  }

  it must "retrieve content server URL when port is missing" in {
    val config = configFor()
    config.contentServerUrl("content01.example.com") must be (new URL(
      "https://content01.example.com:51075/infinityfs/v1/content"))
  }

  def configFor(settings: (String, String)*): InfinityConfig = {
    val defaultSettings = ConfigFactory.load()
    val customSettings = ConfigFactory.parseMap(settings.toMap.asJava)
    val config = customSettings.withFallback(defaultSettings)
    new MetadataServerConfig(config)
  }
}
