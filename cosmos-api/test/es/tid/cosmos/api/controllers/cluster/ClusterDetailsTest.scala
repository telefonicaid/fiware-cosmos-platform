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

package es.tid.cosmos.api.controllers.cluster

import java.sql.Timestamp

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.api.profile.Cluster
import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.{Hdfs, MapReduce2}

class ClusterDetailsTest extends FlatSpec with MustMatchers {

  val clusterOwner = ClusterUser(
    username = "jsmith",
    publicKey = "ssh-rsa XXXXX jsmith@example.com"
  )
  val extraUser = ClusterUser(
    username = "pbanks",
    publicKey = "ssh-rsa ZZZZZ pbanks@example.com"
  )
  val sampleDetails = ClusterDetails(
    href = "http://host/path",
    id = "001",
    name = ClusterName("test cluster"),
    size = 20,
    state = "terminating",
    stateDescription = "releasing resources",
    master = Some(host(1)),
    slaves = Some(Seq(host(2), host(3))),
    users = Some(Seq(clusterOwner, extraUser)),
    services = Set("ServiceB", "ServiceA"),
    blockedPorts = Set(2, 1, 3),
    shared = true
  )
  val sampleJson = Json.obj(
    "href" -> "http://host/path",
    "id" -> "001",
    "name" -> "test cluster",
    "size" -> 20,
    "state" -> "terminating",
    "stateDescription" -> "releasing resources",
    "master" -> hostJson(1),
    "slaves" -> Json.arr(hostJson(2), hostJson(3)),
    "shared" -> true,
    "users" -> Json.arr(
      Json.obj(
        "username" -> "jsmith",
        "sshPublicKey" -> "ssh-rsa XXXXX jsmith@example.com",
        "isSudoer" -> false
      ),
      Json.obj(
        "username" -> "pbanks",
        "sshPublicKey" -> "ssh-rsa ZZZZZ pbanks@example.com",
        "isSudoer" -> false
      )
    ),
    "services" -> Json.arr("ServiceA", "ServiceB"),
    "blockedPorts" -> Json.arr(1, 2, 3)
  )

  "Cluster details" must "be serialized to JSON" in {
    Json.toJson(sampleDetails) must be (sampleJson)
  }

  it must "omit master information when unavailable" in {
    Json.toJson(sampleDetails.copy(master = None)) must equal (sampleJson - "master")
  }

  it must "omit slaves information when unavailable" in {
    Json.toJson(sampleDetails.copy(slaves = None)) must equal (sampleJson - "slaves")
  }

  it must "omit user information when unavailable" in {
    Json.toJson(sampleDetails.copy(users = None)) must equal (sampleJson - "users")
  }

  it must "not show removed users" in {
    val removedUser = extraUser.copy(sshEnabled = false, hdfsEnabled = false)
    val clusterWithRemovedUser = sampleDetails.copy(users = Some(Seq(clusterOwner, removedUser)))
    val jsonWithRemovedUser = sampleJson ++ Json.obj("users" -> Json.arr(
      Json.obj(
        "username" -> "jsmith",
        "sshPublicKey" -> "ssh-rsa XXXXX jsmith@example.com",
        "isSudoer" -> false
      )
    ))
    Json.toJson(clusterWithRemovedUser) must equal (jsonWithRemovedUser)
  }

  it must "not show hidden services" in {
    val description = ImmutableClusterDescription(
      id = ClusterId("id"),
      name = ClusterName("mycluster"),
      size = 2,
      state = Running,
      nameNode = None,
      master = None,
      slaves = Seq.empty,
      users = None,
      services = Set(Hdfs.name, MapReduce2.name) ++ ClusterDetails.unlistedServices,
      blockedPorts = Set.empty
    )
    val assignment = Cluster(description.id, 13L, new Timestamp(0))
    val details = ClusterDetails(description, assignment, "href")
    val listedServices = (Json.toJson(details) \ "services").as[Set[String]]
    ClusterDetails.unlistedServices.foreach { serviceName =>
      listedServices must not contain serviceName
    }
  }

  private def host(index: Int) = HostDetails(s"cosmos0$index", s"192.168.0.$index")

  private def hostJson(index: Int) = Json.obj(
    "hostname" -> s"cosmos0$index",
    "ipAddress" -> s"192.168.0.$index"
  )
}
