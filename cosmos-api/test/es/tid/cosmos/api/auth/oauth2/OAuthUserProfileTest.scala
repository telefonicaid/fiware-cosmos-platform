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
