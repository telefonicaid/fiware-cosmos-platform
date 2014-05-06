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
import java.util
import scala.concurrent._

import org.apache.hadoop.fs.CreateFlag
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols
import org.apache.hadoop.io.EnumSetWritable

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions.Action.{Context, Result}
import es.tid.cosmos.infinity.server.config.InfinityConfig

case class CreateFile(
    config: InfinityConfig,
    nameNode: NamenodeProtocols,
    on: Path,
    permissions: PermissionsMask,
    replication: Option[Short],
    blockSize: Option[Long]) extends Action {

  import ExecutionContext.Implicits.global

  override def apply(context: Context): Future[Result] = for {
    _ <- createFile()
    metadata <- GetMetadata(nameNode, on)(context)
  } yield metadata

  private def createFile(): Future[Unit] = future {
    nameNode.create(
      on.toString,  // src
      permissions.toHadoop,
      "hdfs", // TODO: determine what this parameter is used for
      new EnumSetWritable(util.EnumSet.of(CreateFlag.CREATE)),
      false, //createParent
      replication.getOrElse(config.replication),
      blockSize.getOrElse(config.blockSize))
  }
}
