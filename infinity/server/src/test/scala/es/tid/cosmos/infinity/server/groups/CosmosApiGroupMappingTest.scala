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

package es.tid.cosmos.infinity.server.groups

import java.util
import scala.collection.JavaConverters._
import scala.concurrent.duration._

import org.apache.hadoop.security.GroupMappingServiceProvider
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.MockCosmosApi

class CosmosApiGroupMappingTest extends FlatSpec with MustMatchers {

  "The group mapping" must "get mappings from the Cosmos API" in new Fixture {
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.givenUserGroups("handle", Seq("cosmos"))
    cosmosApi.withServer {
      mapping.groupsFor("handle") must be (Seq("cosmos"))
    }
  }

  it must "return an empty list if authorization is rejected" in new Fixture {
    cosmosApi.givenInfinitySecret("misconfigured_secret")
    cosmosApi.givenUserGroups("handle", Seq("cosmos"))
    cosmosApi.withServer {
      mapping.groupsFor("handle") must be ('empty)
    }
  }

  it must "fallback when handle doesn't exist in Cosmos API" in new Fixture {
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.withServer {
      mapping.groupsFor("handle") must be (fallbackGroups)
    }
  }

  it must "return an empty list when response is malformed" in new Fixture {
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.givenMalformedGroupMapping("handle")
    cosmosApi.withServer {
      mapping.groupsFor("handle") must be ('empty)
    }
  }

  it must "return an empty list when server doesn't respond" in new Fixture {
    mapping.groupsFor("handle") must be ('empty)
  }

  it must "return an empty list when server times out" in new Fixture {
    cosmosApi.givenInfinitySecret(sharedSecret)
    cosmosApi.givenResponseIsDelayed(apiTimeout * 2)
    cosmosApi.givenUserGroups("handle", Seq("cosmos"))
    cosmosApi.withServer {
      mapping.groupsFor("handle") must be ('empty)
    }
  }

  trait Fixture {
    val port = 6783
    val sharedSecret = "shared_secret"
    val fallbackGroups = Seq("hdfs")
    val apiTimeout = 1.seconds
    val mapping = new CosmosApiGroupMapping(
      apiBase = s"http://127.0.0.1:$port",
      infinitySecret = sharedSecret,
      apiTimeout = apiTimeout,
      fallbackMapping = new GroupMappingServiceProvider {
        override def cacheGroupsAdd(groups: util.List[String]): Unit = {}
        override def cacheGroupsRefresh(): Unit = {}
        override def getGroups(user: String): util.List[String] = fallbackGroups.asJava
      }
    )
    val cosmosApi = new MockCosmosApi(port)
  }
}
