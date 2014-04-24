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

package es.tid.cosmos.infinity.common.messages.json

import java.net.URL

import net.liftweb.json.JValue
import net.liftweb.json.Extraction.decompose
import net.liftweb.json.JsonAST.{JArray, JInt, JString}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.{Path, RootPath}
import es.tid.cosmos.infinity.common.messages._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class JsonFormatsTest extends FlatSpec with MustMatchers {

  implicit val formats = JsonFormats

  "JSON formats" must behave like havingMappingsFor(
    RootPath -> JString("/"),
    Path.absolute("/some/path") -> JString("/some/path")
  )
  it must behave like notDeserializingTo[Path](JString("invalid path"), JArray(List.empty))

  it must behave like havingMappingsFor(
    new URL("http://host:8000/some/resource") -> JString("http://host:8000/some/resource")
  )
  it must behave like notDeserializingTo[URL](JString("not a URL"))

  it must behave like havingMappingsFor(
    PermissionsMask.fromOctal("600") -> JString("600"),
    PermissionsMask.fromOctal("777") -> JString("777")
  )
  it must behave like notDeserializingTo[PermissionsMask](JString("7777"), JInt(600))

  it must behave like havingMappingsFor[PathType](File -> JString("file"), Directory -> JString("directory"))
  it must behave like notDeserializingTo[PathType](JString("other"), JInt(12))

  def havingMappingsFor[Value](samples: (Value, JValue)*)(implicit mf: Manifest[Value]): Unit = {
    it must s"have a custom serialization for ${mf.runtimeClass.getSimpleName}" in {
      for ((value, json) <- samples) {
        withClue(s"$value should map to $json:") {
          decompose(value) must be (json)
        }
        withClue(s"$json should map to $value:") {
          json.extract[Value] must be (value)
        }
      }
    }
  }

  def notDeserializingTo[Value](counterExamples: JValue*)(implicit mf: Manifest[Value]): Unit =
    for (counterExample <- counterExamples) {
      it must s"not deserialize $counterExample to ${mf.runtimeClass.getSimpleName}" in {
        counterExample.extractOpt[Value] must be ('empty)
      }
    }
}
