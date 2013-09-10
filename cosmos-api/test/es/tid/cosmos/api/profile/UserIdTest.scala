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

class UserIdTest extends FlatSpec with MustMatchers {

  "A valid user identifier" must "have a non-empty realm" in {
    evaluating { UserId("", "user") } must produce [IllegalArgumentException]
  }

  it must "have a non-empty id" in {
    evaluating { UserId("realm", "") } must produce [IllegalArgumentException]
  }

  it must "have the 'default' realm by default" in {
    UserId("user").realm must be ("default")
  }

  it must "be printed as user@realm" in {
    UserId("realm", "user").toString must be ("user@realm")
  }
}
