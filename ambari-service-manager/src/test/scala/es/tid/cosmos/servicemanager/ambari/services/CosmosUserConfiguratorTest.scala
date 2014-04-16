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

package es.tid.cosmos.servicemanager.ambari.services

import org.scalatest.FlatSpec
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}

import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys

class CosmosUserConfiguratorTest extends FlatSpec with MustMatchers {

  val users = Seq(ClusterUser(
    username = "userName",
    publicKey = "public_key",
    sshEnabled = true,
    hdfsEnabled = false,
    isSudoer = false
  ))
  val properties = Map(
    ConfigurationKeys.MasterNode -> "aNameNodeName"
  )
  var contributions = new CosmosUserConfigurator(users).contributions(properties)

  "A Cosmos user service" must "not contribute to global and core configurations" in {
    contributions.global must be('empty)
    contributions.core must be('empty)
  }

  it must "contribute to service configurations" in {
    contributions.services must have length 1
    val properties = contributions.services(0).properties
    properties.get("user1_ssh_enabled") must be (Some(true))
    properties.get("user1_hdfs_enabled") must be (Some(false))
    properties.get("user1_ssh_master_authorized_keys") must containsSshKey("public_key")
    properties.get("user1_is_sudoer") must be (Some(false))
  }

  private def containsSshKey(key: String) = new Matcher[Option[Any]] {
    def apply(left: Option[Any]) = MatchResult(
      matches = left match {
        case Some(str: String) => str.contains(key)
        case _ => false
      },
      failureMessage = s"property value $left does not contains SSH key $key",
      negatedFailureMessage = s"property value $left contains SSH key $key"
    )
  }
}
