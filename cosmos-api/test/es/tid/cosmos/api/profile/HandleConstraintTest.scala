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

class HandleConstraintTest extends FlatSpec with MustMatchers {

  "Handle constraint" must "reject invalid unix handles" in {
    HandleConstraint("not a unix handle") must be (false)
  }

  it must "reject blacklisted names" in {
    HandleConstraint.blacklist.foreach { handle =>
      HandleConstraint(handle) must be (false)
    }
  }

  it must "accept valid unix names" in {
    HandleConstraint("MrValidHandle123") must be (true)
  }
}
