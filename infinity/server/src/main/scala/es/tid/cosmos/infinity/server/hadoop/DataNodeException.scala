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

import es.tid.cosmos.infinity.common.fs.Path

class DataNodeException(message: String, cause: Throwable) extends Exception(message, cause)

object DataNodeException {
  case class ContentNotFound(path: Path, cause: Throwable = null) extends DataNodeException(
    s"content for $path not found", cause)
}
