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

import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions.Action.OperationNotAllowed

case class ChangeOwner(nameNode: NamenodeProtocols, on: Path, owner: String) extends Action {

  import ExecutionContext.Implicits.global

  override def apply(context: Action.Context): Future[Action.Result] =
    if (!context.user.superuser)
      Future.successful(OperationNotAllowed(context.user.username, on))
    else
      future { nameNode.setOwner(on.toString, owner, ChangeOwner.SameGroup) }
        .flatMap(_ => GetMetadata(nameNode, on).apply(context))
}

object ChangeOwner {
  private val SameGroup = null
}
