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

package es.tid.cosmos.infinity.common.json

import net.liftweb.json.JsonAST.JValue

import es.tid.cosmos.infinity.common.messages.Request

class RequestMessageParser extends JsonParser[Request] {

  /** Parses an ActionMessage from JSON.
    *
    * @param input  Raw JSON
    * @return       A parsed value
    * @throws ParseException  If input cannot be parsed
    */
  override def parse(input: String): Request = {
    val json = parseJson(input)
    extract(json)(manifestFor(actionName(json)))
  }

  private def actionName(json: JValue) = (json \ "action").extractOpt[String]
    .getOrElse(throw ParseException(s"Missing 'action' field in $json"))

  private def manifestFor(name: String) =
    Request.manifestFor(name).getOrElse(throw ParseException(s"Unsupported action '$name'"))
}
