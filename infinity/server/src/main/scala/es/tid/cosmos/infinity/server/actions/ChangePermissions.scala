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

import scala.concurrent._
import scala.concurrent.Future.{successful => success}

import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.common.BooleanFutures._
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions.Action.OperationNotAllowed

case class ChangePermissions(
    nameNode: NamenodeProtocols, on: Path, permissions: PermissionsMask) extends Action {
  import ExecutionContext.Implicits.global

  override def apply(context: Action.Context): Future[Action.Result] = {
    val metadata = new MetadataUtil(nameNode, context)
    lazy val isUserPathOwner_> : Future[Boolean] =
      metadata.forPath(on).map(_.owner == context.user.username)

    val isAllowed_> : Future[Boolean] =  success(context.user.superuser) or  isUserPathOwner_>

    isAllowed_> flatMap { allowed =>
      if (allowed) {
        nameNode.setPermission(on.toString, permissions.toHadoop)
        metadata.action(on)
      }
      else
        success(OperationNotAllowed(context.user.username, on))
    }
  }
}
