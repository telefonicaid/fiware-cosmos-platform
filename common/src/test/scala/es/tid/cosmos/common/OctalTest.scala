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

package es.tid.cosmos.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class OctalTest extends FlatSpec with MustMatchers {

  "Octal parser" must "parse valid strings" in new {
    Octal("777") must be (511)
  }

  it must "support leading zeroes" in new {
    Octal("022") must be (18)
  }

  it must "throw on invalid numbers" in new {
    evaluating(Octal("19")) must produce [NumberFormatException]
  }
}
