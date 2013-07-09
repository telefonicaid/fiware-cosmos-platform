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

import java.lang.IllegalArgumentException

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class QuotaTest extends FlatSpec with MustMatchers {

  "An empty quota" must "reject every request" in {
    EmptyQuota.withinQuota(4) must be (false)
    EmptyQuota.withinQuota(1000) must be (false)
  }

  "An unlimited quota" must "accept every request" in {
    UnlimitedQuota.withinQuota(4) must be (true)
    UnlimitedQuota.withinQuota(1000) must be (true)
  }

  "A finite quota" must "accept request within bounds" in {
    FiniteQuota(5).withinQuota(4) must be (true)
  }

  it must "be valid only for positive values" in {
    evaluating{ FiniteQuota(0) } must produce [IllegalArgumentException]
    evaluating{ FiniteQuota(-1) } must produce [IllegalArgumentException]
  }

  it must "disallow requests out of bounds" in {
    FiniteQuota(5).withinQuota(1000) must be (false)
  }

  "A quota" must "be initialized from optional value" in {
    Quota(None) must equal (UnlimitedQuota)
    Quota(Some(0)) must equal (EmptyQuota)
    Quota(Some(5)) must equal (FiniteQuota(5))
  }
}
