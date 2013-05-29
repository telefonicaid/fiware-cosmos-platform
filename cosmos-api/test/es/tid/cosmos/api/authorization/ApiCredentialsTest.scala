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

package es.tid.cosmos.api.authorization

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ApiCredentialsTest extends FlatSpec with MustMatchers {
  "Api credentials" must "be randomly created on demand" in {
    ApiCredentials.random must not be (ApiCredentials.random)
  }

  it must "preserve API id size invariant" in {
    val ex = evaluating {
      ApiCredentials("small", "justright,justright,justright,justright,")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("API identifier")
  }

  it must "preserve API secret size invariant" in {
    val ex = evaluating {
      ApiCredentials("justright,justright,justright,justright,", "small")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("API secret")
  }
}
