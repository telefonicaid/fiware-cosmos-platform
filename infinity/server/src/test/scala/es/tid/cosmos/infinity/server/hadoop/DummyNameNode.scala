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

package es.tid.cosmos.infinity.server.hadoop

import es.tid.cosmos.infinity.common.permissions.{PermissionsMask, UserProfile}
import es.tid.cosmos.infinity.common.fs.{PathMetadata, Path}

class DummyNameNode extends NameNode {

  override def pathMetadata(path: Path): PathMetadata = ???

  override def createDirectory(
    path: Path, owner: String, group: String, permissions: PermissionsMask): Unit = ???

  override def createFile(
    path: Path, owner: String, group: String, permissions: PermissionsMask,
    replication: Option[Short], blockSize: Option[Long]): Unit = ???

  override def deletePath(path: Path, recursive: Boolean): Unit = ???

  override def movePath(from: Path, to: Path): Unit = ???

  override def setPermissions(path: Path, permissions: PermissionsMask): Unit = ???

  override def setGroup(path: Path, newGroup: String): Unit = ???

  override def setOwner(path: Path, newOwner: String): Unit = ???

  override def as[A](user: UserProfile)(body: => A): A = body
}
