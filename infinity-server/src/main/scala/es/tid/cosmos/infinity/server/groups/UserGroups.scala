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
