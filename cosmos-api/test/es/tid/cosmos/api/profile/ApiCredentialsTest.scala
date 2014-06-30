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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ApiCredentialsTest extends FlatSpec with MustMatchers {
  "Api credentials" must "be randomly created on demand" in {
    ApiCredentials.random must not be ApiCredentials.random()
  }

  it must "preserve API id size invariant" in {
    val ex = evaluating {
      ApiCredentials("small", "justright0justright1justright2justright3")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("API key")
  }

  it must "preserve API secret size invariant" in {
    val ex = evaluating {
      ApiCredentials("justright0justright1", "small")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("API secret")
  }
}
