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

import es.tid.cosmos.infinity.common.fs.{DirectoryMetadata, FileMetadata, PathMetadata}

class MetadataParser extends JsonParser[PathMetadata] {

  /** Parses a file or directory metadata from JSON.
    *
    * @param input  Raw JSON
    * @return       A PathMetadata
    * @throws ParseException  If input cannot be parsed
    */
  def parse(input: String): PathMetadata = {
    val json = parseJson(input)
    (json \ "type").extractOpt[String] match {
      case None => throw ParseException(s"Missing 'type' field in $input")
      case Some("file") => extract[FileMetadata](json)
      case Some("directory") => extract[DirectoryMetadata](json)
      case Some(unsupported) => throw ParseException(s"Unsupported metadata type '$unsupported'")
    }
  }
}
