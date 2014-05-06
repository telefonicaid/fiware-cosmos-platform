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

package es.tid.cosmos.infinity.server.urls

import java.net.URL

import es.tid.cosmos.infinity.common.fs.Path

/** A mapper object able to provide the URLs corresponding to Infinity resources. */
trait UrlMapper {

  /** Retrieve the URL of the given metadata resource. */
  def metadataUrl(path: Path): URL

  /** Retrieve the URL of the the content resource for datanode hostname.
    *
    * This operation can be used to map the HDFS datanode hostnames to the URLs of the wrapping
    * content server. If there is no mapping for the given datanode host, None is returned.
    * 
    * @param path The path of the resource
    * @param datanode The datanode hostname
    * @return The URL of the content resource for the given path in the mapped content server,
    *         or None if there is no such mapping.
    */
  def contentUrl(path: Path, datanode: String): Option[URL]
}
