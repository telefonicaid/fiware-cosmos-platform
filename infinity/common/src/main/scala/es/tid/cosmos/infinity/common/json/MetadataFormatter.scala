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

import net.liftweb.json._
import net.liftweb.json.JsonDSL._

import es.tid.cosmos.infinity.common.fs.{DirectoryMetadata, FileMetadata, PathMetadata}

class MetadataFormatter extends JsonFormatter[PathMetadata] {

  private val directorySize: JObject = "size" -> 0

  /** Formats metadata information as JSON. */
  def format(metadata: PathMetadata): String =
    pretty(render(Extraction.decompose(metadata).merge(extraFields(metadata))))

  /** Extra fields that are not automatically pulled out by the Lift JSON API */
  private def extraFields(metadata: PathMetadata): JObject = {
    val typeField: JObject = "type" -> metadata.`type`.toString
    metadata match {
      case _: FileMetadata => typeField
      case _: DirectoryMetadata => typeField.merge(directorySize)
    }
  }
}
