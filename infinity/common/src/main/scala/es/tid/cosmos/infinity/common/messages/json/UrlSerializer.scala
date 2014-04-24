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
import scala.util.Try

import net.liftweb.json._
import net.liftweb.json.JsonAST.JString

private[json] class UrlSerializer extends Serializer[URL] {

  private val urlClass = classOf[URL]

  override def serialize(implicit format: Formats): PartialFunction[Any, JValue] = {
    case url: URL => JString(url.toString)
  }

  override def deserialize(implicit format: Formats): PartialFunction[(TypeInfo, JValue), URL] = {
    case (TypeInfo(`urlClass`, _), JString(rawUrl)) if isUrl(rawUrl) => new URL(rawUrl)
  }

  private def isUrl(rawUrl: String): Boolean = Try(new URL(rawUrl)).isSuccess
}
