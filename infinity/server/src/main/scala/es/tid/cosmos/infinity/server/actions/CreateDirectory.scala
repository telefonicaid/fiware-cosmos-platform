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

import scala.concurrent.{Future, future}
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions.Action.Context
import es.tid.cosmos.infinity.server.actions.MetadataAction.Created
import es.tid.cosmos.infinity.server.hadoop.NameNode

case class CreateDirectory(
    nameNode: NameNode, on: Path, permissions: PermissionsMask) extends MetadataAction {

  override def apply(context: Context): Future[MetadataAction.Result] = future {
    nameNode.as(context.user) {
      nameNode.createDirectory(on, context.user.username, context.user.groups.head, permissions)
      Created(nameNode.pathMetadata(on))
    }
  }
}
