/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.hadoop

import java.io.{IOException, InputStream}

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.util.ToClose

/** An abstraction for an HDFS Data node service. */
trait DataNode extends UserPrivileges {
  /** Retrieve the file contents corresponding to the given path.
    *
    * @param path   The path whose content is to be obtained
    * @param offset The optional offset from where to start reading the content
    * @param length The optional length of content to read from the given offset
    * @return       The content part as per offset and length.
    *               The content is provided as an autocloseable stream.
    *               Use [[ToClose.useAndClose]] to safely read the content part.
    */
  @throws[IOException]
  def open(path: Path, offset: Option[Long], length: Option[Long]): ToClose[InputStream]

  /** Add content to the existing content of the file located at the given path.
    *
    * @param path          The path of the file where the content is to be added
    * @param contentStream The stream from where to read the content to be added.
    *                      <b>Note:</b> It is left to the client's discretion to close the stream.
    *                      e.g. You can use
    *                      [[es.tid.cosmos.infinity.server.util.IoUtil.withAutoClose]]
    *                      to automatically release the stream.
    */
  @throws[IOException]
  def append(path: Path, contentStream: InputStream): Unit

  /** Overwrite the content of the file located at the given path.
    *
    * @param path          The path of the file where the content is to be written
    * @param contentStream The stream from where to read to content that will ovewrite the file.
    *                      <b>Note:</b> It is left to the client's discretion to close the stream.
    *                      e.g. You can use
    *                      [[es.tid.cosmos.infinity.server.util.IoUtil.withAutoClose]]
    *                      to automatically release the stream.
    */
  @throws[IOException]
  def overwrite(path: Path, contentStream: InputStream): Unit
}
