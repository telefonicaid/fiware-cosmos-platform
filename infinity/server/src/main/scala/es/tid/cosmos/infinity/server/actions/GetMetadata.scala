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

import java.net.URL
import java.util.Date
import scala.concurrent._

import org.apache.hadoop.hdfs.protocol.HdfsFileStatus
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages.FileMetadata
import es.tid.cosmos.infinity.server.util.HdfsConversions._

case class GetMetadata(nameNode: NamenodeProtocols, on: Path) extends Action {

  import ExecutionContext.Implicits.global

  override def apply(context: Action.Context): Future[Action.Result] = future {
    val fileStatus = nameNode.getFileInfo(on.toString)
    Action.PathMetadataResult(fileMetadataOf(context, fileStatus))
  }

  private def fileMetadataOf(
      context: Action.Context, fileStatus: HdfsFileStatus) = FileMetadata(
    path = on,
    metadata = context.urlMapper.metadataUrl(on),
    content = pickContentServer(context, fileStatus),
    owner = fileStatus.getOwner,
    group = fileStatus.getGroup,
    modificationTime = new Date(fileStatus.getModificationTime),
    accessTime = new Date(fileStatus.getAccessTime),
    permissions = fileStatus.getPermission.toInfinity,
    replication = fileStatus.getReplication,
    blockSize = fileStatus.getBlockSize,
    size = fileStatus.getLen
  )

  private def pickContentServer(
      context: Action.Context, fileStatus: HdfsFileStatus): Option[URL] = {
    val blocks = nameNode.getBlockLocations(on.toString, 0, fileStatus.getLen)
    if (blocks.locatedBlockCount() != 0) {
      val locs = blocks.get(0).getLocations
      if (locs.size != 0) context.urlMapper.contentUrl(on, locs(0).getHostName)
      else None
    }
    else None
  }
}
