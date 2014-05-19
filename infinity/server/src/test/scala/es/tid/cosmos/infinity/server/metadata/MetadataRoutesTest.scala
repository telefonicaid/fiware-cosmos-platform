/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.metadata

import java.io.StringReader
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.language.postfixOps
import scala.util.Success

import com.typesafe.config.ConfigFactory
import org.mockito.Matchers.any
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import unfiltered.filter.async.Plan.Intent

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.hadoop.{MockNameNode, NameNodeException, NameNode}
import es.tid.cosmos.infinity.server.routes.RoutesBehavior
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

class MetadataRoutesTest extends FlatSpec
    with RoutesBehavior[NameNode] with MustMatchers with FutureMatchers {

  def newRoutes = new Routes {
    val config = new MetadataServerConfig(ConfigFactory.load())
    val urlMapper = new InfinityUrlMapper(config)
    val hadoopApi = spy(new MockNameNode)
    val someUri = "/infinityfs/v1/metadata/some/uri"
    val somePath = Path.absolute("/some/uri")
    val authService = mock[AuthenticationService]
    val intent: Intent = new MetadataRoutes(
      config,
      authService,
      hadoopApi,
      urlMapper
    ).intent
  }

  "GetMetadata" must behave like {
    supportsAuthorization()
    canHandleNotFound(hadoopBehavior = pathNotFound)
    //TODO: Cross cutting case?
    it should "return 500 on IOErrors" in new Authenticated(identity) {
      doReturn(Future.failed(NameNodeException.IOError()))
        .when(routes.hadoopApi).pathMetadata(any())
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must equal(500)
    }
  }

  "CreateFile" must behave like {
    val toCreateFile: RequestFunction = request => request.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "mkfile",
          |  "name": "somefile.txt",
          |  "permissions": "777",
          |  "replication": 3
          |}
        """.stripMargin)
      )
    supportsAuthorization(requestTransformation = toCreateFile)
    canHandleNotFound(
      requestTransformation = toCreateFile,
      hadoopBehavior = parentDoesNotExist
    )

    it must "return 409 when the path already exists" in new Authenticated(toCreateFile) {
      doReturn(Future.failed(NameNodeException.PathAlreadyExists(Path.absolute("/"))))
        .when(routes.hadoopApi).createFile(any(), any(), any(), any(), any(), any())
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must equal(409)
    }

    it must "return 400 when the body is invalid" in new Authenticated(_.copy(method = "POST")) {
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must equal(400)
    }

    it must "return 422 when the parent is not a directory" in new Authenticated(toCreateFile) {
      doReturn(Future.failed(NameNodeException.ParentNotDirectory(Path.absolute("/"))))
        .when(routes.hadoopApi).createFile(any(), any(), any(), any(), any(), any())
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must equal(422)
    }
  }

  "MoveFile" must behave like {
    val toMoveFile: RequestFunction = request => request.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "move",
          |  "name": "somefile.txt",
          |  "from": "/some/other/place"
          |}
        """.stripMargin)
    )
    supportsAuthorization(requestTransformation = toMoveFile)
    canHandleNotFound(
      requestTransformation = toMoveFile,
      hadoopBehavior = originNotFound
    )
  }

  "ChangeOwner" must behave like {
    val toChown: RequestFunction = request => request.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "chown",
          |  "owner": "heisenberg"
          |}
        """.stripMargin)
    )
    supportsAuthorization(requestTransformation = toChown)
    canHandleNotFound(
      requestTransformation = toChown,
      hadoopBehavior = ownedNotFound
    )
  }

  "ChangeGroup" must behave like {
    val toChangeGroup: RequestFunction = request => request.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "chgrp",
          |  "group": "pollosHermanos"
          |}
        """.stripMargin)
    )
    supportsAuthorization(requestTransformation = toChangeGroup)
    canHandleNotFound(
      requestTransformation = toChangeGroup,
      hadoopBehavior = notFoundForSetGroup
    )
  }

  "Chmod" must behave like {
    val toChmod: RequestFunction = request => request.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "chmod",
          |  "permissions": "775"
          |}
        """.stripMargin)
    )
    supportsAuthorization(requestTransformation = toChmod)
    canHandleNotFound(
      requestTransformation = toChmod,
      hadoopBehavior = notFoundForSetPermissions
    )
  }

  "DeleteFile" must behave like {
    supportsAuthorization(requestTransformation = _.copy(method = "DELETE"))
    canHandleNotFound(hadoopBehavior = pathNotFound)
  }

  def pathNotFound(nameNode: NameNode): Unit = notFoundWhen(nameNode).pathMetadata(any())

  def parentDoesNotExist(nameNode: NameNode): Unit =
    notFoundWhen(nameNode).createFile(any(), any(), any(), any(), any(), any())

  def originNotFound(nameNode: NameNode): Unit = notFoundWhen(nameNode).movePath(any(), any())

  def ownedNotFound(nameNode:NameNode): Unit = notFoundWhen(nameNode).setOwner(any(), any())

  def notFoundForSetGroup(nameNode:NameNode): Unit = notFoundWhen(nameNode).setGroup(any(), any())

  def notFoundForSetPermissions(nameNode:NameNode): Unit =
    notFoundWhen(nameNode).setPermissions(any(), any())

  def notFoundWhen(nameNode: NameNode) =
    doReturn(Future.failed(NameNodeException.NoSuchPath(Path.absolute("/")))).when(nameNode)

}
