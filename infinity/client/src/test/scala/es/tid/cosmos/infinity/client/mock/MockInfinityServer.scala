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

package es.tid.cosmos.infinity.client.mock

import java.net.URL
import java.util.Date

import org.scalatest.Assertions
import unfiltered.jetty.Http

import es.tid.cosmos.infinity.client.mock.ErrorBehavior._
import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class MockInfinityServer(metadataPort: Int, defaultDate: Date) extends Assertions {

  val metadataEndpoint = new URL(s"http://localhost:$metadataPort")

  @volatile
  private var pathsMetadata = Map.empty[Path, PathMetadata]
  @volatile
  private var pathsContents = Map.empty[Path, String]

  private var behavior: Behavior = NormalBehavior

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

  def givenWillReturnBadRequest(): Unit = { behavior = BadRequestBehavior }
  def givenWillReturnGibberish(): Unit = { behavior = GibberishResponseBehavior }
  def givenWillReturnForbidden(): Unit = { behavior = ForbiddenBehavior }
  def givenWillReturnAlreadyExists(): Unit = { behavior = AlreadyExistsBehavior }
  def givenWillReturnNotFound(): Unit = { behavior = NotFoundBehavior }

  def withServer[T](block: => T): T = {
    val server = Http.local(metadataPort)
      .filter(behavior.planFor(pathsMetadata, pathsContents, TestDataFactory))
    try {
      server.start()
      block
    } finally {
      server.stop().join()
    }
  }

  object TestDataFactory extends DataFactory {

    override def fileMetadata(
        path: Path,
        permissions: PermissionsMask,
        replication: Option[Short] = None,
        blockSize: Option[Long] = None): FileMetadata =
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

    override def dirMetadata(path: Path, permissions: PermissionsMask): DirectoryMetadata =
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

    override def metadataUrl(path: Path): URL = new URL(s"$metadataEndpoint$MetadataPreamble$path")
    override def contentUrl(path: Path): URL = new URL(s"$metadataEndpoint$ContentPreamble$path")
  }
}
