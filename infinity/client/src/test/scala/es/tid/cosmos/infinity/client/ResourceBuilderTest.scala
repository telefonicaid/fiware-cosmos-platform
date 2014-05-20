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

package es.tid.cosmos.infinity.client

import java.net.URL

import com.ning.http.client.Request
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.{BasicAuth, BearerToken}
import es.tid.cosmos.infinity.common.credentials.{ClusterCredentials, Credentials, UserCredentials}
import es.tid.cosmos.infinity.common.fs.Path

class ResourceBuilderTest extends FlatSpec with MustMatchers {

  val endpoint = new URL("https://infinity:8080")
  val path = Path.absolute("/some/path")
  val location = new URL("https://data97:8888/infinityfs/v1/content/some/path")

  def builder(credentials: Credentials = UserCredentials("key", "secret")) =
    new ResourceBuilder(endpoint, credentials)

  def authHeader(request: Request): String = request.getHeaders.getFirstValue("Authorization")

  "A resource builder" must "create a metadata resource" in {
    val resource = builder().metadata(path).build()
    resource.getUrl must be ("https://infinity:8080/infinityfs/v1/metadata/some/path")
    authHeader(resource) must be (BasicAuth("key", "secret"))
  }

  it must "create a content resource" in {
    val resource = builder().content(location).build()
    resource.getUrl must be ("https://data97:8888/infinityfs/v1/content/some/path")
    authHeader(resource) must be (BasicAuth("key", "secret"))
  }

  it must "authenticate the resource with cluster credentials" in {
    val credentials = ClusterCredentials(origin = null, clusterSecret = "secret")
    authHeader(builder(credentials).metadata(path).build()) must be (BearerToken("secret"))
    authHeader(builder(credentials).content(location).build()) must be (BearerToken("secret"))
  }
}
