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

package es.tid.cosmos.api.oauth2

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class UserProfileTest extends FlatSpec with MustMatchers {
  trait FullProfile {
    val profile = UserProfile("id", Some("John Smith"), Some("jsmith@tid.es"))
  }

  trait PartialProfiles {
    val missingName = UserProfile("id", None, Some("jsmith@tid.es"))
    val missingEmail = UserProfile("id", Some("John Smith"), None)
  }

  trait EmptyProfile {
    val profile = UserProfile("id")
  }

  "A full user profile" must "provide a contact with all fields" in new FullProfile {
    profile.contact must be ("John Smith (jsmith@tid.es)")
  }

  "A partial user profile" must "provide a contact with existing fields" in new PartialProfiles {
    missingName.contact must be ("(jsmith@tid.es)")
    missingEmail.contact must be ("John Smith")
  }

  "An empty user profile" must "show a placeholder as contact" in new EmptyProfile {
    profile.contact must be ("--")
  }
}
