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

package es.tid.cosmos.api.auth.oauth2

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.UserId

class OAuthUserProfileTest extends FlatSpec with MustMatchers {

  val id = UserId("realm", "id")

  trait FullProfile {
    val profile = OAuthUserProfile(id, Some("John Smith"), Some("jsmith@tid.es"))
  }

  trait PartialProfiles {
    val missingName = OAuthUserProfile(id, None, Some("jsmith@tid.es"))
    val missingEmail = OAuthUserProfile(id, Some("John Smith"), None)
  }

  trait EmptyProfile {
    val profile = OAuthUserProfile(id)
  }

  "A full user profile" must "provide a contact identifying the user" in new FullProfile {
    profile.contact must be ("John Smith")
  }

  "A partial user profile" must "provide a contact with the name when possible" in
    new PartialProfiles {
      missingName.contact must be ("(jsmith@tid.es)")
      missingEmail.contact must be ("John Smith")
    }

  "An empty user profile" must "show a placeholder based on id as contact" in new EmptyProfile {
    profile.contact must be ("(id@realm)")
  }
}
