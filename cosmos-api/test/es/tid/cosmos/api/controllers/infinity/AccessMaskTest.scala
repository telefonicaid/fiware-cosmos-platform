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

package es.tid.cosmos.api.controllers.infinity

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
