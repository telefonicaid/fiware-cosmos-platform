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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import java.net.InetAddress

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.credentials.{ClusterCredentials, UserCredentials}
import es.tid.cosmos.infinity.common.permissions.{PermissionsMask, UserProfile}
import es.tid.cosmos.infinity.server.MockCosmosApi

class CosmosApiAuthenticationServiceTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Cosmos API authorization" must "retrieve a profile given user credentials" in new Fixture {
    val profile = UserProfile(
      username = "john",
      groups = Seq("scientists"),
      mask = PermissionsMask.fromOctal("777")
    )
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.givenProfile(userCredentials, profile)
    cosmosApi.withServer {
      client.authenticate(userCredentials) must eventually(be(profile))
    }
  }

  it must "fail if authentication service is not available" in new Fixture {
    val credentials = UserCredentials("key", "secret")
    client.authenticate(credentials) must
      eventuallyFailWith("Cannot access the authentication service")
  }

  it must "fail if profile is malformed" in new Fixture {
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.givenMalformedProfile(userCredentials)
    cosmosApi.withServer {
      client.authenticate(userCredentials) must eventuallyFailWith("Cannot parse profile")
    }
  }

  it must "fail if infinity secret is misconfigured" in new Fixture {
    cosmosApi.givenInfinitySecret("other_secret")
    cosmosApi.withServer {
      client.authenticate(userCredentials) must eventuallyFailWith("Authentication was rejected")
    }
  }

  it must "fail if the user profile is accessed from out of the whitelist" in new Fixture {
    val clusterCredentials = ClusterCredentials(
      origin = InetAddress.getByName("10.0.0.1"),
      clusterSecret = "secret"
    )
    val credentialsWithBadOrigin = clusterCredentials.copy(origin = InetAddress.getByName("10.0.0.2"))
    val profile = UserProfile(
      username = "john",
      groups = Seq("scientists"),
      mask = PermissionsMask.fromOctal("770"),
      accessFrom = Some(Set("10.0.0.1"))
    )
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.givenProfile(clusterCredentials, profile)
    cosmosApi.withServer {
      client.authenticate(clusterCredentials) must eventually(be(profile))
      client.authenticate(credentialsWithBadOrigin) must eventuallyFailWith("Invalid access")
    }
  }

  trait Fixture {
    val port = 6784
    val sharedSecret = "shared_secret"
    val userCredentials = UserCredentials("key", "secret")
    val cosmosApi = new MockCosmosApi(port)
    val client = new CosmosApiAuthenticationService(
      apiBase = s"http://localhost:$port",
      infinitySecret = sharedSecret,
      superGroup = "hdfs"
    )
  }
}
