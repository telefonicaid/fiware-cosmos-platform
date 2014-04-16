/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
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
