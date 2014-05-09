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

import org.scalatest.Assertions
import unfiltered.filter.{Plan, Planify}
import unfiltered.request.{Path => UPath, _}
import unfiltered.response._

import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.json._
import es.tid.cosmos.infinity.common.messages._
import es.tid.cosmos.infinity.common.messages.Request._

object NormalBehavior extends Behavior with Formatters with Assertions {

  override def planFor(
      paths: Map[Path, PathMetadata], contents: Map[Path, String], dataFactory: DataFactory): Plan = {
    import dataFactory._

    val MetadataPath = s"""$MetadataPreamble(.*)""".r
    val ContentPath = s"""$ContentPreamble(.*)""".r
    val metadata = new InMemoryInfinityMetadata(paths, contents, dataFactory)
    val actionParser = new RequestMessageParser

    def handleActionOnPath(path: Path, action: Request) = {
      action match {
        case CreateFile(name, permissions, replication, blockSize) =>
          val filePath = path / name
          statusFromPathChecks(filePath).getOrElse {
            metadata.createPath(filePath, fileMetadata(filePath, permissions, replication, blockSize))
            Created
          }

        case CreateDirectory(name, permissions) =>
          val dirPath = path / name
          statusFromPathChecks(dirPath).getOrElse {
            metadata.createPath(dirPath, dirMetadata(dirPath, permissions))
            Created
          }

        case Move(name, from) =>
          metadata.move(from / name, path / name)
          Created

        case ChangeOwner(owner) =>
          metadata.changeOwner(path, owner)
          NoContent

        case ChangeGroup(group) =>
          metadata.changeGroup(path, group)
          NoContent

        case ChangePermissions(permissions) =>
          metadata.changePermissions(path, permissions)
          NoContent

        case _ => BadRequest
      }
    }

    def statusFromPathChecks(path: SubPath) =
      if (metadata.get(path.parentPath).isEmpty) Some(NotFound)
      else if (metadata.get(path).isDefined) Some(Conflict)
      else None

    Planify.apply {
      case GET(UPath(MetadataPath(path))) =>
        metadata.get(Path.absolute(path)).fold(NotFound ~> errorBody("not found")) { metadata =>
          Ok ~> ResponseString(metadataFormatter.format(metadata))
        }

      case GET(UPath(ContentPath(path))) =>
        metadata.getContents(Path.absolute(path)).fold(NotFound ~> errorBody("not found")) { contents =>
          Ok ~> ResponseString(contents)
        }

      case req@POST(UPath(MetadataPath(path))) =>
        handleActionOnPath(Path.absolute(path), actionParser.parse(Body.string(req)))

      case req@POST(UPath(ContentPath(path))) =>
        val absolutePath = Path.absolute(path)
        if (metadata.get(absolutePath).isDefined) {
          metadata.appendToFile(absolutePath, Body.string(req))
          NoContent
        } else NotFound

      case req@PUT(UPath(ContentPath(path))) =>
        val absolutePath = Path.absolute(path)
        if (metadata.get(absolutePath).isDefined) {
          metadata.setContents(absolutePath, Body.string(req))
          NoContent
        } else NotFound

      case DELETE(UPath(MetadataPath(path))) =>
        metadata.delete(Path.absolute(path))
        NoContent

      case unexpected =>
        println(s"Unexpected request to ${unexpected.uri}")
        fail(s"Unexpected request to ${unexpected.uri}")
    }
  }
}
