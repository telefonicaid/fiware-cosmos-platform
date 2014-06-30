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

package es.tid.cosmos.infinity.common.json.formats

import net.liftweb.json.{Formats, JValue, Serializer, TypeInfo}
import net.liftweb.json.JsonAST.JString
import es.tid.cosmos.infinity.common.fs.PathType

private[formats] class PathTypeSerializer extends Serializer[PathType] {

  private val typeClass = classOf[PathType]
  private val validTypes = Set(PathType.File, PathType.Directory).map(_.toString)

  override def serialize(implicit format: Formats): PartialFunction[Any, JValue] = {
    case pathType: PathType => JString(pathType.toString)
  }

  override def deserialize(implicit format: Formats): PartialFunction[(TypeInfo, JValue), PathType] = {
    case (TypeInfo(clazz, _), JString(raw)) if typeClass.isAssignableFrom(clazz) && validType(raw) =>
      PathType.valueOf(raw)
  }

  private def validType(raw: String): Boolean = validTypes.contains(raw.toLowerCase)
}
