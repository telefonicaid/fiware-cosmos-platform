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

package es.tid.comos.infinity.client

import java.net.URL
import java.util.Date

import javax.servlet.http.HttpServletResponse
import org.scalatest.Assertions
import unfiltered.filter.{Plan, Planify}
import unfiltered.jetty.Http
import unfiltered.request.{Path => UPath, _}
import unfiltered.response._

import es.tid.cosmos.infinity.common.{Path, SubPath}
import es.tid.cosmos.infinity.common.messages._
import es.tid.cosmos.infinity.common.messages.Action._
import es.tid.cosmos.infinity.common.messages.json._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class MockInfinityServer(metadataPort: Int, defaultDate: Date) extends Assertions {
  import DataFactory._

  val metadataEndpoint = new URL(s"http://localhost:$metadataPort")

  private var pathsMetadata = Map.empty[Path, PathMetadata]
  private var pathsContents = Map.empty[Path, String]

  private val metadataFormatter = new MetadataFormatter()
  private val errorFormatter = new ErrorDescriptorFormatter()
  private val actionParser = new ActionMessageParser

  private val MetadataPreamble = "/infinityfs/v1/metadata"
  private val MetadataPath = s"""$MetadataPreamble(.*)""".r
  private val ContentPreamble = "/infinityfs/v1/content"
  private val ContentPath = s"""$ContentPreamble(.*)""".r

  private val normalBehavior = Planify.apply {
    case GET(UPath(MetadataPath(path))) =>
      pathsMetadata.get(Path.absolute(path)).fold(NotFound ~> errorBody("not found")) { metadata =>
        Ok ~> ResponseString(metadataFormatter.format(metadata))
      }

    case GET(UPath(ContentPath(path))) =>
      pathsContents.get(Path.absolute(path)).fold(NotFound ~> errorBody("not found")) { contents =>
        Ok ~> ResponseString(contents)
      }

    case req @ POST(UPath(MetadataPath(path))) => {
      handleActionOnPath(Path.absolute(path), actionParser.parse(Body.string(req)))
    }

    case DELETE(UPath(MetadataPath(path))) => {
      val absolutePath = Path.absolute(path)
      pathsMetadata -= absolutePath
      pathsContents -= absolutePath
      NoContent
    }

    case unexpected =>
      println(s"Unexpected request to ${unexpected.uri}")
      fail(s"Unexpected request to ${unexpected.uri}")
  }
  private var behavior: Plan = normalBehavior

  def givenExistingPaths(metadata: PathMetadata*): Unit = {
    for (meta <- metadata) {
      pathsMetadata += meta.path -> meta
    }
  }

  def givenExistingContent(metadata: FileMetadata, content: String): Unit = {
    require(
      pathsMetadata(metadata.path) == metadata,
      "File metadata not found. Check that it was included in givenExistingPaths")
    pathsContents += metadata.path -> content
  }

  def givenWillReturnBadRequest(): Unit = {
    behavior = Planify.apply {
      case _ => BadRequest ~> errorBody("don't understand you")
    }
  }

  def givenWillReturnGibberish(): Unit = {
    behavior = Planify.apply {
      case _ => Ok ~> ResponseString("dlalkdkldijodjd")
    }
  }

  def givenWillReturnForbidden(): Unit = {
    behavior = Planify.apply {
      case _ => Forbidden ~> errorBody("don't dare trying that")
    }
  }

  def givenWillReturnAlreadyExists(): Unit = {
    behavior = Planify.apply {
      case _ => Conflict ~> errorBody("file or directory already exists")
    }
  }

  def givenWillReturnNotFound(): Unit = {
    behavior = Planify.apply {
      case _ => NotFound ~> errorBody("file or directory does not exist")
    }
  }

  def withServer[T](block: => T): T = {
    val server = Http.local(metadataPort).filter(behavior)
    try {
      server.start()
      block
    } finally {
      server.stop().join()
    }
  }

  private def errorBody(description: String, code: String = "0") =
    ResponseString(errorFormatter.format(ErrorDescriptor(code, description)))

  private def handleActionOnPath(
    path: Path, action: Action): ResponseFunction[HttpServletResponse] = {

    def statusFromPathChecks(thePath: SubPath) =
      if (!pathsMetadata.contains(thePath.parentPath))
        Some(NotFound)
      else if (pathsMetadata.contains(thePath))
        Some(Conflict)
      else
        None

    action match {
      case CreateFile(name, permissions, replication, blockSize) => {
        val filePath = path / name
        statusFromPathChecks(filePath).getOrElse {
          pathsMetadata += filePath -> fileMetadata(filePath, permissions, replication, blockSize)
          Created
        }
      }

      case CreateDirectory(name, permissions) => {
        val dirPath = path / name
        statusFromPathChecks(dirPath).getOrElse {
          pathsMetadata += dirPath -> dirMetadata(dirPath, permissions)
          Created
        }
      }

      case Move(name, from) => {
        pathsMetadata += path / name -> pathsMetadata(from / name).moved(path / name)
        pathsMetadata -= from / name
        Created
      }

      case ChangeOwner(owner) => {
        pathsMetadata = pathsMetadata.updated(path, pathsMetadata(path).chown(owner))
        NoContent
      }

      case ChangeGroup(group) => {
        pathsMetadata = pathsMetadata.updated(path, pathsMetadata(path).chgroup(group))
        NoContent
      }

      case ChangePermissions(permissions) => {
        pathsMetadata = pathsMetadata.updated(path, pathsMetadata(path).chmod(permissions))
        NoContent
      }

      case _ => BadRequest
    }
  }

  private implicit class MetadataOps(metadata: PathMetadata) {
    def moved(to: SubPath): PathMetadata = metadata.transform(
      onFile = _.copy(path = to, metadata = metadataUrl(to), content = contentUrl(to)),
      onDir  = _.copy(path = to, metadata = metadataUrl(to))
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

  object DataFactory {

    def fileMetadata(
        path: Path,
        permissions: PermissionsMask,
        replication: Option[Int] = None,
        blockSize: Option[Int] = None): FileMetadata =
      FileMetadata(
        path,
        metadata = metadataUrl(path),
        content = contentUrl(path),
        owner = "hdfs",
        group = "hdfs",
        modificationTime = defaultDate,
        accessTime = defaultDate,
        permissions,
        replication = replication.getOrElse(3),
        blockSize = blockSize.getOrElse(2048),
        size = 0
      )

    def dirMetadata(path: Path, permissions: PermissionsMask): DirectoryMetadata =
      DirectoryMetadata(
        path,
        metadata = metadataUrl(path),
        content = Seq.empty,
        owner = "hdfs",
        group = "hdfs",
        modificationTime = defaultDate,
        accessTime = defaultDate,
        permissions
      )

    def metadataUrl(path: Path): URL = new URL(s"$metadataEndpoint$MetadataPreamble$path")
    def contentUrl(path: Path): URL = new URL(s"$metadataEndpoint$ContentPreamble$path")
  }
}
