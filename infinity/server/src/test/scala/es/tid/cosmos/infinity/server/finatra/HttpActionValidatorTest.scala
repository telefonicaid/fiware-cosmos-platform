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

import scalaz.{Failure, Success}

import com.twitter.finagle.http.Request
import com.typesafe.config.ConfigFactory
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols
import org.jboss.netty.handler.codec.http.{HttpVersion, DefaultHttpRequest, HttpMethod}
import org.scalatest.{FlatSpec, Inside}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.server.actions.{Delete, CreateFile, GetMetadata}
import es.tid.cosmos.infinity.server.config.InfinityConfig
import org.jboss.netty.buffer.ChannelBuffers
import java.nio.charset.Charset
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HttpActionValidatorTest extends FlatSpec with MustMatchers with Inside with MockitoSugar {

  private val nameNode = mock[NamenodeProtocols]
  private val config = new InfinityConfig(ConfigFactory.load())
  private val instance = new HttpActionValidator(config, nameNode)

  "Valid HTTP Action" must "fail to extract from an unknown path" in {
    val req = makeRequest(HttpMethod.GET, "/this/is/an/invalid/path")
    inside(instance(req)) {
      case Failure(RequestParsingException.InvalidResourcePath(path, _)) =>
        path must be ("/this/is/an/invalid/path")
    }
  }

  it must "extract a GetMetadata action" in {
    val req = makeRequest(HttpMethod.GET, "/infinityfs/v1/metadata/path/to/file")
    instance(req) must be (Success(GetMetadata(nameNode, Path.absolute("/path/to/file"))))
  }

  it must "extract a CreateFile action" in {
    val req = makeRequest(HttpMethod.POST, "/infinityfs/v1/metadata/path/to/file",
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
        path must be (Path.absolute("/path/to/file/enemies.csv"))
        perms must be (PermissionsMask.fromOctal("640"))
        rep must be (Some(2))
        bsize must be (Some(67108864l))
    }
  }

  it must "extract a Delete action" in {
    val req = makeRequest(HttpMethod.DELETE, "/infinityfs/v1/metadata/path/to/file?recursive=true")
    instance(req) must be (
      Success(Delete(nameNode, Path.absolute("/path/to/file"), recursive = true)))
  }

  private def makeRequest(method: HttpMethod, uri: String, content: Option[String] = None): Request = {
    val req = new DefaultHttpRequest(HttpVersion.HTTP_1_1, method, uri)
    content.foreach { c =>
      req.setContent(ChannelBuffers.copiedBuffer(c, Charset.forName("UTF-8")))
    }
    Request(req)
  }

  private def makeRequest(method: HttpMethod, uri: String, content: String): Request  =
    makeRequest(method, uri, Option(content))
}
