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

import java.io.IOException
import scala.concurrent.{ExecutionContext, Future}

import com.typesafe.config.ConfigFactory
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols
import org.mockito.BDDMockito.given
import org.mockito.Matchers._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.actions.Action.{Context, MoveUnsuccessful, PathMetadataResult}
import es.tid.cosmos.infinity.server.config.InfinityConfig
import es.tid.cosmos.infinity.server.urls.UrlMapper

class MoveFileTest extends FlatSpec with MustMatchers with MockitoSugar with FutureMatchers {

  import ExecutionContext.Implicits.global

  "Move action" must "return file metadata upon successful move completion" in new Fixture {
    given(nameNode.rename(anyString, anyString)).willReturn(true)
    given(meta.action(any[Context], any[Path])).willReturn(Future(metadata))
    moveFile(context) must eventually (be (metadata))
  }

  it must "fail if backend rejects operation" in new Fixture {
    given(nameNode.rename(anyString, anyString)).willReturn(false)
    moveFile(context) must eventually (be (MoveUnsuccessful))
  }

  it must "fail if backend throw exception" in new Fixture {
    given(nameNode.rename(anyString, anyString)).willThrow(new IOException)
    moveFile(context) must eventuallyFailWith[IOException]
  }

  trait Fixture {
    val nameNode = mock[NamenodeProtocols]
    val urlMapper = mock[UrlMapper]
    val meta = mock[MetadataUtil]
    val context = Context(UserProfile("bob", "users"), urlMapper)
    val config = new InfinityConfig(ConfigFactory.load())

    val metadata = PathMetadataResult(null)

    val moveFile = MoveFile(config, nameNode,
      Path.absolute("/from/file"), Path.absolute("/to/file"), meta)
  }
}
