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

package es.tid.cosmos.api.auth.oauth2.keyrock

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

class KeyrockProfileTest extends FlatSpec with MustMatchers {

  "A keyrock profile" must "be converted to an OAuthUserProfile" in {
    val profile = KeyrockProfile(
      id = 10,
      actorId = 120,
      nickName = "johnny",
      displayName = "John Smith",
      email = "jsmith@host.com"
    )
    profile.asUserProfile("domain") must be (OAuthUserProfile(
      UserId("domain", "120"),
      Some("John Smith"),
      Some("jsmith@host.com")
    ))
  }
}
