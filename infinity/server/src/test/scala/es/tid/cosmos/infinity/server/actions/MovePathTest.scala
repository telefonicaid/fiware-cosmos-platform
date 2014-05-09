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
import org.mockito.BDDMockito.given
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
    given(nameNode.movePath(from, to)).willReturn(Future.successful(()))
    given(nameNode.pathMetadata(to)).willReturn(Future.successful(metadata))
    moveFile(context) must eventually (be (metadataResult))
  }

  it must "fail if name node fails to move" in new Fixture {
    given(nameNode.movePath(from, to)).willReturn(
      Future.failed(new NameNodeException.IOError(new Exception("cannot move"))))
    moveFile(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  it must "fail if name node fails to retrieve new metadata" in new Fixture {
    given(nameNode.movePath(from, to)).willReturn(Future.successful(()))
    given(nameNode.pathMetadata(to)).willReturn(
      Future.failed(new NameNodeException.IOError(new Exception("cannot retrieve metadata"))))
    moveFile(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  trait Fixture {
    val from = Path.absolute("/from/file")
    val to = Path.absolute("/to/file")
    val nameNode = mock[NameNode]
    val urlMapper = mock[UrlMapper]
    val context = Action.Context(UserProfile("bob", Seq("users")), urlMapper)
    val config = new MetadataServerConfig(ConfigFactory.load())

    val metadata = mock[FileMetadata]
    val metadataResult = Action.Moved(metadata)

    val moveFile = MovePath(config, nameNode, to, from)
  }
}
