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

package es.tid.cosmos.api.auth.oauth2.github

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

class GitHubProfileTest extends FlatSpec with MustMatchers {


  "A GitHub profile" must "be converted to a generic user profile" in {
    val profile = GitHubProfile(53, "jsmith", "John Smith", "jsmith@tid.es")
    profile.asUserProfile("realm") must equal (OAuthUserProfile(
      UserId("realm", "53"), Some("John Smith"), Some("jsmith@tid.es")))
  }
}
