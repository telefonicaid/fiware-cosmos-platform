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

package es.tid.cosmos.api.controllers.storage

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class AccessMaskTest extends FlatSpec with MustMatchers {

  "An access mask" must "be in the 0000 to 0777 range" in {
    for (invalidValue <- Seq(-1, 777, 512)) {
      evaluating {
        AccessMask(invalidValue)
      } must produce [IllegalArgumentException]
    }
  }

  it must "be formatted in octal" in {
    AccessMask(0).toString must be ("000")
    AccessMask(488).toString must be ("750")
    AccessMask(511).toString must be ("777")
  }

  it must "be parsed from string representation" in {
    AccessMask("777") must be (AccessMask(511))
  }
}
