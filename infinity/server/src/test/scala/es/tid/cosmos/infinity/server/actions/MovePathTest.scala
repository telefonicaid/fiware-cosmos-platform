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

import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path

class MovePathTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Move action" must "return file metadata upon successful move completion" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).movePath(from, on)
    doReturn(Future.successful(metadata)).when(nameNode).pathMetadata(on)
    moveFile(context) must eventually (be (metadataResult))
  }

  it must "fail if name node fails to move" in new Fixture {
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot move"))))
      .when(nameNode).movePath(from, on)
    moveFile(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  it must "fail if name node fails to retrieve new metadata" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).movePath(from, on)
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot retrieve metadata"))))
      .when(nameNode).pathMetadata(on)
    moveFile(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  trait Fixture extends ActionFixture {
    val from = Path.absolute("/from/file")
    val metadataResult = MetadataAction.Moved(metadata)
    val moveFile = MovePath(config, nameNode, on, from)
  }
}
