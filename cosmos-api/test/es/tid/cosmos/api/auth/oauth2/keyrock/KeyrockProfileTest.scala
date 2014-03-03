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
