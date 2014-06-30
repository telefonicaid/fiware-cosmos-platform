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

import java.io.InputStream
import scala.concurrent.Future

import es.tid.cosmos.infinity.common.fs.{Path, PathMetadata}
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.urls.UrlMapper
import es.tid.cosmos.infinity.server.util.ToClose

/** An action performed on a Infinity path. */
sealed trait Action[Result <: Action.Result] {

  val on: Path

  def apply(context: Action.Context): Future[Result]
}

object Action {

  case class Context(user: UserProfile, urlMapper: UrlMapper)

  /** A marking trait used to represent the result of an action.
    *
    * The purpose of this marking trait is to have a renderer capable to convert the result
    * into a response according to the protocol used by Infinity Server (e.g., RESTful API).
    */
  sealed trait Result
}

trait MetadataAction extends Action[MetadataAction.Result]

object MetadataAction {
  sealed trait Result extends Action.Result

  /** A file or directory metadata was successfully retrieved. */
  case class Retrieved(metadata: PathMetadata) extends Result

  /** A file or directory was successfully created. */
  case class Created(metadata: PathMetadata) extends Result

  /** A file or directory was successfully moved. */
  case class Moved(metadata: PathMetadata) extends Result

  /** A file or directory was successfully deleted. */
  case class Deleted(metadata: PathMetadata) extends Result

  /** A file or directory owner was successfully set. */
  case class OwnerSet(metadata: PathMetadata) extends Result

  /** A file or directory group was successfully set. */
  case class GroupSet(metadata: PathMetadata) extends Result

  /** A file or directory permissions were successfully set. */
  case class PermissionsSet(metadata: PathMetadata) extends Result
}

trait ContentAction extends Action[ContentAction.Result]

object ContentAction {
  sealed trait Result extends Action.Result

  /** A file content response with the input stream to read from and the resources to release */
  case class Found(stream: ToClose[InputStream]) extends Result

  /** A content was successfully appended to a file. */
  case class Appended(path: Path) extends Result

  /** A file's content was successfully overwritten. */
  case class Overwritten(path: Path) extends Result
}
