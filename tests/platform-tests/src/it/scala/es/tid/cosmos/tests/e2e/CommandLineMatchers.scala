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

package es.tid.cosmos.tests.e2e

import org.scalatest.matchers.{MatchResult, Matcher}

trait CommandLineMatchers {

  /** A matcher that checks that a command return code is success. */
  val runSuccessfully: Matcher[Int] = new Matcher[Int] {

    override def apply(returnCode: Int) = new MatchResult(
      matches = returnCode == 0,
      failureMessage = s"command failed with exit status $returnCode",
      negatedFailureMessage = s"command doesn't failed with exist status $returnCode"
    )
  }
}
