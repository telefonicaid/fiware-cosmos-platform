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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState._

class CosmosProfileTest extends FlatSpec with MustMatchers {

  "A cosmos profile" must "have a valid unix handle" in {
    val ex = evaluating {
      CosmosProfile(0, Enabled, "id-invalid", EmptyQuota, ApiCredentials.random(), keys = Seq())
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Invalid handle")
  }

  it must "have keys with unique names" in {
    val ex = evaluating {
      CosmosProfile(0, Enabled, "handle", EmptyQuota, ApiCredentials.random(), keys = Seq(
        NamedKey("duplicated", "ssh-rsa AAAA handle@localhost"),
        NamedKey("duplicated", "ssh-rsa BBBB handle@localhost"),
        NamedKey("normal", "ssh-rsa CCCC handle@localhost")
      ))
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Duplicated keys: duplicated")
  }

  it must "have valid SSH keys" in {
    val ex = evaluating {
      CosmosProfile(0, Enabled, "handle", EmptyQuota, ApiCredentials.random(), keys = Seq(
        NamedKey("normal", "invalid")
      ))
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Not a valid signature: invalid")
  }
}
