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

import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.messages.ErrorDescriptor

class ErrorDescriptorTest extends FlatSpec with MustMatchers {

  val error = ErrorDescriptor("101", "Great problem about a missing '\"'")
  val jsonError: JObject = ("code" -> "101") ~ ("cause" -> "Great problem about a missing '\"'")
  val formatter = new ErrorDescriptorFormatter()
  val parser = new ErrorDescriptorParser()

  "An error descriptor" must "be serialized to JSON" in {
    parse(formatter.format(error)) must be (jsonError)
  }

  it must "be parsed from JSON" in {
    parser.parse(compact(render(jsonError))) must be (error)
  }

  it must "not be parsed if some field is missing" in {
    evaluating {
      parser.parse("""{"code": "101"}""")
    } must produce [ParseException]
  }
}
