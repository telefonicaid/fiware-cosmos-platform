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
