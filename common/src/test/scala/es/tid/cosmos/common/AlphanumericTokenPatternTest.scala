/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
