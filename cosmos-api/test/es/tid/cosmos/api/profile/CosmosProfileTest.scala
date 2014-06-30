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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.api.quota.{NoGroup, EmptyQuota}

class CosmosProfileTest extends FlatSpec with MustMatchers {

  val validProfile = CosmosProfile(
    id = 0,
    state = Enabled,
    handle = "handle",
    email = "user@example.com",
    group = NoGroup,
    quota = EmptyQuota,
    capabilities = UntrustedUserCapabilities,
    apiCredentials = ApiCredentials.random(),
    keys = Seq(NamedKey("normal", "ssh-rsa CCCC handle@localhost"))
  )

  "A cosmos profile" must "have a valid unix handle" in {
    val ex = evaluating {
      validProfile.copy(handle = "id-invalid")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Invalid handle")
  }

  it must "have keys with unique names" in {
    val ex = evaluating {
      validProfile.copy(keys = Seq(
        NamedKey("duplicated", "ssh-rsa AAAA handle@localhost"),
        NamedKey("duplicated", "ssh-rsa BBBB handle@localhost"),
        NamedKey("normal", "ssh-rsa CCCC handle@localhost")
      ))
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Duplicated keys: duplicated")
  }

  it must "have valid SSH keys" in {
    val ex = evaluating {
      validProfile.copy(keys = Seq(NamedKey("normal", "invalid")))
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Not a valid signature: invalid")
  }
}
