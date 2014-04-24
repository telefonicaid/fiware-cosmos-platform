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

import es.tid.cosmos.infinity.common.messages.json.formats.JsonFormats

/** Base trait for JSON parsers. */
trait JsonParser[Value] {

  /** Parses a value from JSON.
    *
    * @param input  Raw JSON
    * @return       A parsed value
    * @throws ParseException  If input cannot be parsed
    */
  def parse(input: String): Value

  protected implicit val formats = JsonFormats

  protected def parseJson(input: String) =
    JsonParser.parseOpt(input).getOrElse(throw ParseException(s"Malformed JSON: $input"))

  protected def extract[T](json: JValue)(implicit mf: scala.reflect.Manifest[T]): T = try {
    json.extract[T]
  } catch {
    case ex: MappingException =>
      throw ParseException(s"Cannot map JSON to ${mf.runtimeClass.getSimpleName}", ex)
  }
}
