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

class AlphanumericTokenPatternTest extends FlatSpec with MustMatchers {

  "An alphanumeric token pattern" must "require strictly positive length" in {
    for (invalidLength <- Seq(-1, 0)) {
      evaluating(
        new AlphanumericTokenPattern("Pattern", invalidLength)
      ) must produce [IllegalArgumentException]
    }
  }

  val testPattern = new AlphanumericTokenPattern("Test pattern", 10)

  it must "produce tokens of the target length" in {
    consistently {
      testPattern.generateRandom() must have length 10
    }
  }

  it must "produce only alphanumeric characters" in {
    consistently {
      testPattern.generateRandom().filterNot(_.isLetterOrDigit) must be ('empty)
    }
  }

  it must "produce valid tokens" in {
    consistently {
      testPattern.requireValid(testPattern.generateRandom())
    }
  }

  it must "detect invalid tokens" in {
    for (invalidToken <- Seq("short", "too long token", "012345678ñ", "0 2 4 5 8 ")) {
      evaluating {
        testPattern.requireValid(invalidToken)
      } must produce [IllegalArgumentException]
    }
  }

  private def consistently(block: => Unit): Unit = for (_ <- 1 to 1000) { block }
}
