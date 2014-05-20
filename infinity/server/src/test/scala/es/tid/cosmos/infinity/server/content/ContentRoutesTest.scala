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

package es.tid.cosmos.infinity.server.content

import java.io.{ByteArrayInputStream, InputStream}
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.language.postfixOps
import scala.util.Success

import com.typesafe.config.ConfigFactory
import org.mockito.Matchers.{any, eq =>the}
import org.mockito.Mockito.{doReturn, spy}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import unfiltered.filter.async.Plan.Intent
import unfiltered.response.{NoContent, Ok}

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.{DummyDataNode, DataNodeException, DataNode}
import es.tid.cosmos.infinity.server.routes.RoutesBehavior
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper
import es.tid.cosmos.infinity.server.util.ToClose

class ContentRoutesTest extends FlatSpec
    with RoutesBehavior[DataNode] with MustMatchers with FutureMatchers {

  def newRoutes: Routes = new Routes {
    val config = new ContentServerConfig(ConfigFactory.load())
    val urlMapper = new InfinityUrlMapper(config)
    val hadoopApi = spy(new DummyDataNode)
    val someUri = "/infinityfs/v1/content/some/uri"
    val somePath = Path.absolute("/some/uri")
    val authService = mock[AuthenticationService]
    val intent: Intent = new ContentRoutes(
      config,
      authService,
      hadoopApi,
      urlMapper
    ).intent
  }

  "GetContent" must behave like {
    routeSupportingAuthorization()
    routeHandlingNotFound(hadoopBehavior = openFileNotFound)

    it must "correctly route an authorized and supported request" in new WithContent {
      doReturn(Future.successful(ToClose(inputStream)))
        .when(routes.hadoopApi).open(the(routes.somePath), any[Option[Long]], any[Option[Long]])
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must be (Ok.code)
      baseResponse.body must be (body)
    }
  }

  "AppendContent" must behave like {
    val toPost: RequestFunction = request => request.copy(method = "POST")
    routeSupportingAuthorization(requestTransformation = toPost)
    routeHandlingNotFound(
      requestTransformation = toPost,
      hadoopBehavior = appendFileNotFound
    )

    it must "correctly route an authorized request on existing file" in new WithContent(toPost) {
      doReturn(Future.successful())
        .when(routes.hadoopApi).append(the(routes.somePath), any[InputStream])
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must be (NoContent.code)
    }
  }

  "OverwriteContent" must behave like {
    val toPut: RequestFunction = request => request.copy(method = "PUT")
    routeSupportingAuthorization(requestTransformation = toPut)
    routeHandlingNotFound(
      requestTransformation = toPut,
      hadoopBehavior = overwriteFileNotFound
    )

    it must "correctly route an authorized request on existing file" in new WithContent(toPut) {
      doReturn(Future.successful())
        .when(routes.hadoopApi).overwrite(the(routes.somePath), any[InputStream])
      routes.intent.apply(responder) must be (Success())
      responder.response_> must runUnder(1 second)
      baseResponse._status must be (NoContent.code)
    }
  }

  def openFileNotFound(dataNode: DataNode): Unit = {
    doReturn(Future.failed(DataNodeException.FileNotFound(Path.absolute("/"))))
      .when(dataNode).open(any[Path], any[Option[Long]], any[Option[Long]])
  }

  def appendFileNotFound(dataNode: DataNode): Unit = {
    doReturn(Future.failed(DataNodeException.FileNotFound(Path.absolute("/"))))
      .when(dataNode).append(any[Path], any[InputStream])
  }

  def overwriteFileNotFound(dataNode: DataNode): Unit = {
    doReturn(Future.failed(DataNodeException.FileNotFound(Path.absolute("/"))))
      .when(dataNode).overwrite(any[Path], any[InputStream])
  }

  abstract class WithContent(requestTransformation: RequestFunction = identity)
      extends Authenticated(requestTransformation) {
    val body = "aBody"
    val inputStream = new ByteArrayInputStream(body.getBytes)
  }
}
