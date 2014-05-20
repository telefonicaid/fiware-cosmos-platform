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

import java.net.URL
import java.util.Date

import org.mockito.BDDMockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import unfiltered.request.HttpRequest
import unfiltered.response.ResponseFunction

import es.tid.cosmos.infinity.common.fs.{FileMetadata, Path}
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions.MetadataAction._
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse

class ActionResultHttpRendererTest extends FlatSpec with MustMatchers with MockitoSugar {

  val request = mockRequest

  val metadata = FileMetadata(
    path = Path.absolute("/path/to/file"),
    content = Some(new URL("http://content.example.com/path/to/file")),
    owner = "gandalf",
    group = "istari",
    metadata = new URL("http://meta.example.com/path/to/file"),
    modificationTime = new Date(10000),
    accessTime = new Date(10001),
    permissions = PermissionsMask.fromOctal("755"),
    replication = 3,
    blockSize = 512,
    size = 2048
  )

  "Action resut HTTP renderer" must "render Retrieved" in {
    val rep = ActionResultHttpRenderer(request, Retrieved(metadata))
    val response = getResponse(rep)
    response._status must be (200)
    response.body must include ("/path/to/file")
  }

  it must "render Created" in {
    val rep = ActionResultHttpRenderer(request, Created(metadata))
    val response = getResponse(rep)
    response._status must be (201)
    response.body must include ("/path/to/file")
  }

  it must "render Moved" in {
    val rep = ActionResultHttpRenderer(request, Moved(metadata))
    val response = getResponse(rep)
    response._status must be (201)
    response.body must include ("/path/to/file")
  }

  it must "render Deleted" in {
    val rep = ActionResultHttpRenderer(request, Deleted(metadata))
    val response = getResponse(rep)
    response._status must be (204)
    response.body must be ('empty)
  }

  it must "render OwnerSet" in {
    val rep = ActionResultHttpRenderer(request, OwnerSet(metadata))
    val response = getResponse(rep)
    response._status must be (204)
    response.body must be ('empty)
  }

  it must "render GroupSet" in {
    val rep = ActionResultHttpRenderer(request, GroupSet(metadata))
    val response = getResponse(rep)
    response._status must be (204)
    response.body must be ('empty)
  }

  it must "render PermissionsSet" in {
    val rep = ActionResultHttpRenderer(request, PermissionsSet(metadata))
    val response = getResponse(rep)
    response._status must be (204)
    response.body must be ('empty)
  }

  private def getResponse(res: ResponseFunction[Any]) =
    res.apply(new MockHttpResponse[Any]).asInstanceOf[MockHttpResponse[Any]]

  private def mockRequest: HttpRequest[_] = {
    val req = mock[HttpRequest[_]]
    given(req.method).willReturn("GET")
    given(req.uri).willReturn("/path/to/resource")
    given(req.headers("Authentication")).willReturn(Seq("Basic AAA:BBB").iterator)
    req
  }
}
