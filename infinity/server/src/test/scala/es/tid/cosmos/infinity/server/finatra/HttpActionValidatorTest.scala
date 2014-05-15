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

package es.tid.cosmos.infinity.server.finatra

import java.nio.charset.Charset
import scala.util.{Success, Failure}

import com.twitter.finagle.http.Request
import com.typesafe.config.ConfigFactory
import org.jboss.netty.buffer.ChannelBuffers
import org.jboss.netty.handler.codec.http.{DefaultHttpRequest, HttpMethod, HttpVersion}
import org.scalatest.{FlatSpec, Inside}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.NameNode

class HttpActionValidatorTest extends FlatSpec with MustMatchers with Inside with MockitoSugar {

  private val nameNode = mock[NameNode]
  private val config = new MetadataServerConfig(ConfigFactory.load())
  private val instance = new HttpActionValidator(config, nameNode)
  private val someUri = "/infinityfs/v1/metadata/path/to/file"
  private val somePath = Path.absolute("/path/to/file")

  "Valid HTTP Action" must "fail to extract from an unknown path" in {
    val req = makeRequest(HttpMethod.GET, "/this/is/an/invalid/path")
    inside(instance(req)) {
      case Failure(RequestParsingException.InvalidResourcePath(path, _)) =>
        path must be ("/this/is/an/invalid/path")
    }
  }

  it must "extract a GetMetadata action" in {
    val req = makeRequest(HttpMethod.GET, someUri)
    instance(req) must be (Success(GetMetadata(nameNode, somePath)))
  }

  it must "extract a CreateFile action" in {
    val req = makeRequest(HttpMethod.POST, someUri,
      """
        |{
        |  "action" : "mkfile",
        |  "name" : "enemies.csv",
        |  "permissions" : "640",
        |  "replication" : 2,
        |  "blockSize" : 67108864
        |}
      """.stripMargin)
    inside(instance(req)) {
      case Success(CreateFile(_, _, path, perms, rep, bsize)) =>
        path must be (somePath / "enemies.csv")
        perms must be (PermissionsMask.fromOctal("640"))
        rep must be (Some(2))
        bsize must be (Some(67108864l))
    }
  }

  it must "extract a CreateDirectory action" in {
    val req = makeRequest(HttpMethod.POST, someUri,
      """
        |{
        |  "action" : "mkdir",
        |  "name" : "enemies",
        |  "permissions" : "750"
        |}
      """.stripMargin)
    inside(instance(req)) {
      case Success(CreateDirectory(_, path, perms)) =>
        path must be (somePath / "enemies")
        perms must be (PermissionsMask.fromOctal("750"))
    }
  }

  it must "extract a Delete action" in {
    val req = makeRequest(HttpMethod.DELETE, s"$someUri?recursive=true")
    instance(req) must be (
      Success(DeletePath(nameNode, somePath, recursive = true)))
  }

  it must "extract a ChangeOwner action" in {
    val req = makeRequest(HttpMethod.POST, someUri,
      """
        |{
        |  "action" : "chown",
        |  "owner" : "theoden",
        |}
      """.stripMargin)
    instance(req) must be (
      Success(ChangeOwner(nameNode, somePath, "theoden"))
    )
  }

  it must "extract a ChangeGroup action" in {
    val req = makeRequest(HttpMethod.POST, someUri,
      """
        |{
        |  "action" : "chgrp",
        |  "group" : "valar",
        |}
      """.stripMargin)
    instance(req) must be (
      Success(ChangeGroup(nameNode, somePath, "valar"))
    )
  }

  it must "extract a ChangePermission action" in {
    val req = makeRequest(HttpMethod.POST, someUri,
      """
        |{
        |  "action" : "chmod",
        |  "permissions" : "755",
        |}
      """.stripMargin)
    instance(req) must be (
      Success(ChangePermissions(nameNode, somePath, PermissionsMask.fromOctal("755")))
    )
  }

  it must "extract a MoveFile action" in {
    val req = makeRequest(HttpMethod.POST, "/infinityfs/v1/metadata/path/to/file",
      """
        |{
        |  "action" : "move",
        |  "name" : "enemies.csv",
        |  "from" : "/some/other/file.csv"
        |}
      """.stripMargin)
    inside(instance(req)) {
      case Success(MovePath(_, _, path, from)) =>
        path must be (Path.absolute("/path/to/file/enemies.csv"))
        from must be (Path.absolute("/some/other/file.csv"))
    }
  }

  private def makeRequest(
      method: HttpMethod, uri: String, content: Option[String] = None): Request = {
    val req = new DefaultHttpRequest(HttpVersion.HTTP_1_1, method, uri)
    content.foreach { c =>
      req.setContent(ChannelBuffers.copiedBuffer(c, Charset.forName("UTF-8")))
    }
    Request(req)
  }

  private def makeRequest(method: HttpMethod, uri: String, content: String): Request  =
    makeRequest(method, uri, Option(content))
}
