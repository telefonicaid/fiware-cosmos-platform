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

package es.tid.cosmos.infinity.server.actions

import scala.concurrent.{ExecutionContext, Future}

import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.infinity.common.fs.{Path, PathMetadata}
import es.tid.cosmos.infinity.server.actions.Action.{Context, PathMetadataResult}

/** Utility for the [[GetMetadata]] action to allow other actions to easily read metadata. */
trait MetadataUtil {

  /** Get the metadata for the given path.
    *
    * @param context  the context within which the actions will take place.
    * @param path the path whose metadata to get
    * @return     the path metadata iff they exist
    */
  def forPath(context: Context, path: Path): Future[PathMetadata]

  /** Perform the action on a given path
    *
    * @param context  the context within which the actions will take place.
    * @param path the path whose metadata to get
    * @return     the action result
    */
  def action(context: Context, path: Path): Future[Action.Result]
}

/** Builder for [[MetadataUtil]] instances. */
object MetadataUtil {
  def apply(nameNode: NamenodeProtocols) =
    new MetadataUtilGetAction(nameNode)
}

/**
 * [[MetadataUtil]] implementation based on Get action.
 *
 * @param nameNode the nameNode protocols
 */
private[actions] class MetadataUtilGetAction(
  nameNode: NamenodeProtocols) extends MetadataUtil {

  import ExecutionContext.Implicits.global

  override def forPath(context: Context, path: Path): Future[PathMetadata] =
    for (PathMetadataResult(metadata) <-  action(context, path)) yield metadata

  override def action(context: Context, path: Path): Future[Action.Result] =
    GetMetadata(nameNode, path).apply(context)
}
