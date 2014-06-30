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

package es.tid.cosmos.infinity.server.groups

import net.liftweb.json
import net.liftweb.json._

private[groups] object UserGroups {

  case class InvalidUserGroups(message: String, cause: Throwable = null)
    extends IllegalArgumentException(message, cause)

  /** Parse the JSON response of Cosmos API for the user group mapping.
    *
    * @param input  Input JSON
    * @return       List of groups in the same order as in the input
    * @throws InvalidUserGroups If the input can't be parsed
    */
  def parse(input: String): Seq[String] = parseJsonObject(input) match {
    case JObject(List(JField("groups", JArray(groups)))) => groups.map(parseGroup)
    case _ => throw InvalidUserGroups("Invalid JSON: 'groups' is not an array field")
  }

  private def parseJsonObject(input: String) = try {
    json.parse(input)
  } catch {
    case ex: JsonParser.ParseException => throw InvalidUserGroups("Not a JSON", ex)
  }

  private def parseGroup(input: JValue): String = input match {
    case JString(group) => group
    case value => throw InvalidUserGroups(s"Invalid JSON: cannot parse group $value")
  }
}
