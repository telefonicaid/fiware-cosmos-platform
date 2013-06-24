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

package es.tid.cosmos.servicemanager.ambari.rest.as

import net.liftweb.json.JsonDSL._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class FlatValuesTest extends FlatSpec with MustMatchers {

  "A FlatValues extractor" must "extract the child value of multiple parent attributes" in {
    val json = ("top" ->
      ("parentList" -> List(
        ("parentAttribute1" -> ("child" -> "value1") ~ ("anotheChild" -> "anotherValue1")),
        ("parentAttribute1" -> ("child" -> "value2") ~ ("anotheChild" -> "anotherValue2"))
      ))
    )
    FlatValues(json, "parentList", "child") must equal(List("value1", "value2"))
  }

  it must "extract the child value when only 1 single parent attribute" in {
    val json = ("top" ->
      ("parentList" ->
        List(("parentAttribute1" -> ("child" -> "value1") ~ ("anotheChild" -> "anotherValue1")))))
    FlatValues(json, "parentList", "child") must equal(List("value1"))
  }
}
