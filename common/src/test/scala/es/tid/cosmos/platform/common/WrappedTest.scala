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

package es.tid.cosmos.platform.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class WrappedTest extends FlatSpec with MustMatchers {
  "Wrapped" must "extract the cause of an exception" in {
    val innerEx = new NoSuchElementException("missing foo")
    val outerEx = new RuntimeException("something went wrong", innerEx)
    Wrapped.unapply(outerEx) must be (Some(innerEx))
  }

  it must "extract nothing from exceptions without cause" in {
    val simpleEx = new RuntimeException("simply failed")
    Wrapped.unapply(simpleEx) must not be ('defined)
  }

  it must "extract nothing from null" in {
    Wrapped.unapply(null) must not be ('defined)
  }
}
