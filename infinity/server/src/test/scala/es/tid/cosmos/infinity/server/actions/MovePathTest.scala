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
package es.tid.cosmos.infinity.server.actions

import scala.concurrent.Future

import com.typesafe.config.ConfigFactory
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.{FileMetadata, Path}
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.urls.UrlMapper

class MovePathTest extends FlatSpec with MustMatchers with MockitoSugar with FutureMatchers {

  "Move action" must "return file metadata upon successful move completion" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).movePath(from, to)
    doReturn(Future.successful(metadata)).when(nameNode).pathMetadata(to)
    moveFile(context) must eventually (be (metadataResult))
  }

  it must "fail if name node fails to move" in new Fixture {
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot move"))))
      .when(nameNode).movePath(from, to)
    moveFile(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  it must "fail if name node fails to retrieve new metadata" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).movePath(from, to)
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot retrieve metadata"))))
      .when(nameNode).pathMetadata(to)
    moveFile(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  trait Fixture {
    val from = Path.absolute("/from/file")
    val to = Path.absolute("/to/file")
    val nameNode = spy(new MockNameNode)
    val urlMapper = mock[UrlMapper]
    val user = UserProfile("bob", Seq("users"))
    val context = Action.Context(user, urlMapper)
    val config = new MetadataServerConfig(ConfigFactory.load())

    val metadata = mock[FileMetadata]
    val metadataResult = Action.Moved(metadata)

    val moveFile = MovePath(config, nameNode, to, from)
  }
}
