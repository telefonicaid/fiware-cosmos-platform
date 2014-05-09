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

package es.tid.cosmos.infinity.client.mock

import scala.concurrent.stm._

import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class InMemoryInfinityMetadata(
    initialPaths: Map[Path, PathMetadata],
    initialContents: Map[Path, String],
    dataFactory: DataFactory) extends MustMatchers {

  import dataFactory._

  private val paths = Ref(initialPaths)
  private val contents = Ref(initialContents)

  def get(path: Path): Option[PathMetadata] = atomic { implicit tx =>
    paths().get(path)
  }

  def getContents(path: Path): Option[String] = atomic { implicit tx =>
    for {
      metadata <- paths().get(path) if !metadata.isDirectory
      result <- contents().get(path)
    } yield result
  }

  def createPath(path: Path, metadata: PathMetadata): Unit = atomic { implicit tx =>
    paths().keySet must not contain path
    paths() = paths() + (path -> metadata)
  }

  def move(from: SubPath, to: SubPath): Unit = atomic { implicit tx =>
    val metadata = paths().apply(from)
    paths() = (paths() - from) + (to -> metadata.moved(to))
  }

  def changeOwner(path: Path, owner: String): Unit = modifyMetadata(path) { _.chown(owner) }
  def changeGroup(path: Path, group: String): Unit = modifyMetadata(path) { _.chgroup(group) }
  def changePermissions(path: Path, mask: PermissionsMask): Unit =
    modifyMetadata(path) { _.chmod(mask) }

  def appendToFile(path: Path, extraContents: String): Unit = atomic { implicit tx =>
    val currentContents = contents()
    contents() = currentContents.updated(path, currentContents(path) + extraContents)
  }

  def setContents(path: Path, newContents: String): Unit = atomic { implicit tx =>
    contents() = contents() + (path -> newContents)
  }

  def delete(path: Path): Unit = atomic { implicit tx =>
    paths() = paths() - path
    contents() = contents() - path
  }

  private def modifyMetadata(path: Path)(transformation: PathMetadata => PathMetadata): Unit =
    atomic { implicit tx =>
      val currentPaths = paths()
      paths() = currentPaths.updated(path, transformation(currentPaths(path)))
    }

  private implicit class MetadataOps(metadata: PathMetadata) {
    def moved(to: SubPath): PathMetadata = metadata.transform(
      onFile = _.copy(path = to, metadata = dataFactory.metadataUrl(to), content = Some(contentUrl(to))),
      onDir  = _.copy(path = to, metadata = dataFactory.metadataUrl(to))
    )

    def chown(owner: String): PathMetadata =
      metadata.transform(_.copy(owner = owner), _.copy(owner = owner))

    def chgroup(group: String): PathMetadata =
      metadata.transform(_.copy(group = group), _.copy(group = group))

    def chmod(permissions: PermissionsMask): PathMetadata =
      metadata.transform(_.copy(permissions = permissions), _.copy(permissions = permissions))

    def transform(
        onFile: FileMetadata => FileMetadata,
        onDir: DirectoryMetadata => DirectoryMetadata): PathMetadata =
      metadata match {
        case f: FileMetadata => onFile(f)
        case d: DirectoryMetadata=> onDir(d)
      }
  }
}
