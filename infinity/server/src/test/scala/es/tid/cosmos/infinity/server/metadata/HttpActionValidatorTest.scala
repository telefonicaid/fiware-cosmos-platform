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

package es.tid.cosmos.infinity.server.metadata

import java.io._
import scala.util.{Success, Failure}

import com.typesafe.config.ConfigFactory
import org.scalatest.{FlatSpec, Inside}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.{RootPath, Path}
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.NameNode
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest

class HttpActionValidatorTest extends FlatSpec with MustMatchers with Inside with MockitoSugar {

  private val nameNode = mock[NameNode]
  private val config = new MetadataServerConfig(ConfigFactory.load())
  private val instance = new HttpActionValidator(config, nameNode)
  private val someUri = "/infinityfs/v1/metadata/path/to/file"
  private val somePath = Path.absolute("/path/to/file")
  private val baseRequest = MockHttpRequest(uri = someUri)
  private val getRequest = baseRequest.copy(method = "GET")
  private val postRequest = baseRequest.copy(method = "POST")
  private val deleteRequest = baseRequest.copy(method = "DELETE")

  "Valid HTTP Action" must "fail to extract from an unknown path" in {
    inside(instance(getRequest.copy(uri = "/this/is/an/invalid/path"))) {
      case Failure(RequestParsingException.InvalidResourcePath(path, _)) =>
        path must be ("/this/is/an/invalid/path")
    }
  }

  it must "extract a GetMetadata action for a subpath" in {
    instance(getRequest) must be (Success(GetMetadata(nameNode, somePath)))
  }

  it must "extract a GetMetadata action for the root path" in {
    val getRootPath = Success(GetMetadata(nameNode, RootPath))
    instance(getRequest.copy(uri = "/infinityfs/v1/metadata/")) must be (getRootPath)
    instance(getRequest.copy(uri = "/infinityfs/v1/metadata")) must be (getRootPath)
  }

  it must "extract a CreateFile action" in {
    val content = """
                     |{
                     |  "action" : "mkfile",
                     |  "name" : "enemies.csv",
                     |  "permissions" : "640",
                     |  "replication" : 2,
                     |  "blockSize" : 67108864
                     |}
                     """.stripMargin
    inside(instance(postRequest.copy(reader = makeBody(content)))) {
      case Success(CreateFile(_, _, path, perms, rep, bsize)) =>
        path must be (somePath / "enemies.csv")
        perms must be (PermissionsMask.fromOctal("640"))
        rep must be (Some(2))
        bsize must be (Some(67108864l))
    }
  }

  it must "extract a CreateDirectory action" in {
    val content = """
                    |{
                    |  "action" : "mkdir",
                    |  "name" : "enemies",
                    |  "permissions" : "750"
                    |}
                  """.stripMargin
    inside(instance(postRequest.copy(reader = makeBody(content)))) {
      case Success(CreateDirectory(_, path, perms)) =>
        path must be (somePath / "enemies")
        perms must be (PermissionsMask.fromOctal("750"))
    }
  }

  it must "extract a Delete action" in {
    instance(deleteRequest.copy(params = Map("recursive" -> Seq("true")))) must be (
      Success(DeletePath(nameNode, somePath, recursive = true)))
  }

  it must "extract a ChangeOwner action" in {
    val content = """
                    |{
                    |  "action" : "chown",
                    |  "owner" : "theoden",
                    |}
                  """.stripMargin
    instance(postRequest.copy(reader = makeBody(content))) must be (
      Success(ChangeOwner(nameNode, somePath, "theoden"))
    )
  }

  it must "extract a ChangeGroup action" in {
    val content = """
                    |{
                    |  "action" : "chgrp",
                    |  "group" : "valar",
                    |}
                  """.stripMargin
    instance(postRequest.copy(reader = makeBody(content))) must be (
      Success(ChangeGroup(nameNode, somePath, "valar"))
    )
  }

  it must "extract a ChangePermission action" in {
    val content = """
                    |{
                    |  "action" : "chmod",
                    |  "permissions" : "755",
                    |}
                  """.stripMargin
    instance(postRequest.copy(reader = makeBody(content))) must be (
      Success(ChangePermissions(nameNode, somePath, PermissionsMask.fromOctal("755")))
    )
  }

  it must "extract a MoveFile action" in {
    val content = """
                    |{
                    |  "action" : "move",
                    |  "name" : "enemies.csv",
                    |  "from" : "/some/other/file.csv"
                    |}
                  """.stripMargin
    inside(instance(postRequest.copy(reader = makeBody(content)))) {
      case Success(MovePath(_, _, path, from)) =>
        path must be (Path.absolute("/path/to/file/enemies.csv"))
        from must be (Path.absolute("/some/other/file.csv"))
    }
  }

  private def makeBody(content: String): Reader =
    new StringReader(content)
}
