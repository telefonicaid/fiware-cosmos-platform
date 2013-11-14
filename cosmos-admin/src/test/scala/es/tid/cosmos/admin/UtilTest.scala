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

package es.tid.cosmos.admin

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import scala.concurrent._
import ExecutionContext.Implicits.global

class UtilTest extends FlatSpec with MustMatchers with MockitoSugar {

  "The waitUntilReady method" must "wait for the future to finish successfully" in {
    Util.waitUntilReady(Future { Thread.sleep(2000) }) must be (true)
  }

  it must "wait for the future to finish with error" in {
    Util.waitUntilReady(Future { Thread.sleep(2000); throw new Exception("foo") }) must be (false)
  }
}
