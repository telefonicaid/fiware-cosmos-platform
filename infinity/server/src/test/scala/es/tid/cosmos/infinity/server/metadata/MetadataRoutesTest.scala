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
import es.tid.cosmos.infinity.server.hadoop.{DummyNameNode, NameNodeException, NameNode}
import es.tid.cosmos.infinity.server.routes.RoutesBehavior
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

class MetadataRoutesTest extends FlatSpec
    with RoutesBehavior[NameNode] with MustMatchers with FutureMatchers {

  def newRoutes = new Routes {
    val config = new MetadataServerConfig(ConfigFactory.load())
    val urlMapper = new InfinityUrlMapper(config)
    val hadoopApi = spy(new DummyNameNode)
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
    routeSupportingAuthorization()
    routeHandlingNotFound(hadoopBehavior = givenPathNotFound)
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
    routeSupportingAuthorization(requestTransformation = toCreateFile)
    routeHandlingNotFound(
      requestTransformation = toCreateFile,
      hadoopBehavior = givenParentDoesNotExist
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
    routeSupportingAuthorization(requestTransformation = toMoveFile)
    routeHandlingNotFound(
      requestTransformation = toMoveFile,
      hadoopBehavior = givenOriginNotFound
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
    routeSupportingAuthorization(requestTransformation = toChown)
    routeHandlingNotFound(
      requestTransformation = toChown,
      hadoopBehavior = givenOwnedNotFound
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
    routeSupportingAuthorization(requestTransformation = toChangeGroup)
    routeHandlingNotFound(
      requestTransformation = toChangeGroup,
      hadoopBehavior = givenNotFoundForSetGroup
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
    routeSupportingAuthorization(requestTransformation = toChmod)
    routeHandlingNotFound(
      requestTransformation = toChmod,
      hadoopBehavior = givenNotFoundForSetPermissions
    )
  }

  "DeleteFile" must behave like {
    routeSupportingAuthorization(requestTransformation = _.copy(method = "DELETE"))
    routeHandlingNotFound(hadoopBehavior = givenPathNotFound)
  }

  def givenPathNotFound(nameNode: NameNode): Unit = givenNotFoundWhen(nameNode).pathMetadata(any())

  def givenParentDoesNotExist(nameNode: NameNode): Unit =
    givenNotFoundWhen(nameNode).createFile(any(), any(), any(), any(), any(), any())

  def givenOriginNotFound(nameNode: NameNode): Unit =
    givenNotFoundWhen(nameNode).movePath(any(), any())

  def givenOwnedNotFound(nameNode:NameNode): Unit =
    givenNotFoundWhen(nameNode).setOwner(any(), any())

  def givenNotFoundForSetGroup(nameNode:NameNode): Unit = 
    givenNotFoundWhen(nameNode).setGroup(any(), any())

  def givenNotFoundForSetPermissions(nameNode:NameNode): Unit =
    givenNotFoundWhen(nameNode).setPermissions(any(), any())

  def givenNotFoundWhen(nameNode: NameNode) =
    doReturn(Future.failed(NameNodeException.NoSuchPath(Path.absolute("/")))).when(nameNode)

}
