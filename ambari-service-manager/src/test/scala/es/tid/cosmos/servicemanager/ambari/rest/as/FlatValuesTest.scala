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
