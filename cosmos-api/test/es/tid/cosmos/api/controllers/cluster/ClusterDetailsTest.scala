/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.api.controllers.cluster

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.HostDetails

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
    name = "test cluster",
    size = 20,
    state = "terminating",
    stateDescription = "releasing resources",
    master = Some(host(1)),
    slaves = Some(Seq(host(2), host(3))),
    users = Some(Seq(clusterOwner, extraUser))
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
    )
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

  private def host(index: Int) = HostDetails(s"cosmos0$index", s"192.168.0.$index")

  private def hostJson(index: Int) = Json.obj(
    "hostname" -> s"cosmos0$index",
    "ipAddress" -> s"192.168.0.$index"
  )
}
