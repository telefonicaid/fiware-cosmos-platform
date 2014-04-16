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

package es.tid.cosmos.infinity.server.actions

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.fs.PathMetadata
import es.tid.cosmos.infinity.server.util.Path

/** An action performed on a Infinity path. */
trait Action {
  val on: Path

  def apply(user: UserProfile): Action.Result
}

object Action {

  /** A marking trait used to represent the result of an action.
    *
    * The purpose of this marking trait is to have a renderer capable to convert the result
    * into a response according to the protocol used by Infinity Server (e.g., RESTful API).
    */
  sealed trait Result

  /** A file or directory metadata object resulting from an action. */
  case class PathMetadataResult(metadata: PathMetadata) extends Result

  /** A directory metadata object resulting from an action. */
  case class DirectoryMetadataResult(metadata: PathMetadata) extends Result

  /** A result of an action indicating that there is no such path. */
  case class NoSuchPath(path: Path) extends Result
}
