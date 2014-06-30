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

package es.tid.cosmos.infinity.server.urls

import java.net.URL

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.fs.RootPath
import es.tid.cosmos.infinity.server.config.InfinityConfig

class InfinityUrlMapperTest extends FlatSpec with MustMatchers {

  class InfinityConfigStub(trailingStashes: Boolean) extends InfinityConfig {

    val pathSuffix = if (trailingStashes) "/" else ""

    override val metadataBaseUrl: URL =
      new URL("https://infinity:1234/infinity/v1/metadata" + pathSuffix)
    override def contentServerUrl(hostname: String): URL =
      new URL("https", hostname, 4321, "/infinity/v1/content" + pathSuffix)

    override def metadataProtocol: String = ???
    override def metadataPort: Int = ???
    override def metadataBasePath: String = ???
    override def metadataHost: String = ???
  }

  val mapper = new InfinityUrlMapper(new InfinityConfigStub(trailingStashes = false))
  val mapperWithTrailingSlashes =
    new InfinityUrlMapper(new InfinityConfigStub(trailingStashes = true))
  val path = RootPath / "some" / "path"

  "An URL mapper" must "map a metadata URL" in {
    val expectedUrl = new URL("https://infinity:1234/infinity/v1/metadata/some/path")
    mapper.metadataUrl(path) must be (expectedUrl)
    mapperWithTrailingSlashes.metadataUrl(path) must be (expectedUrl)
  }

  it must "map a content URL" in {
    val contentHost = "data78"
    val expectedUrl = new URL("https://data78:4321/infinity/v1/content/some/path")
    mapper.contentUrl(path, contentHost) must be (expectedUrl)
    mapperWithTrailingSlashes.contentUrl(path, contentHost) must be (expectedUrl)
  }
}
