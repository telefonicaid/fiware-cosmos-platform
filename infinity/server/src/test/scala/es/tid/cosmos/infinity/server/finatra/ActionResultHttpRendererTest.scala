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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.infinity.common.fs.{Path, FileMetadata}
import java.net.URL
import java.util.Date
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.actions.Action
import org.jboss.netty.handler.codec.http.HttpResponseStatus

class ActionResultHttpRendererTest extends FlatSpec with MustMatchers {

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

  "Action resut HTTP renderer" must "render Action.Retrieved" in {
    val rep = ActionResultHttpRenderer(Action.Retrieved(metadata)).build
    rep.status must be (HttpResponseStatus.OK)
    rep.getContentString() must include ("/path/to/file")
  }

  it must "render Action.Created" in {
    val rep = ActionResultHttpRenderer(Action.Created(metadata)).build
    rep.status must be (HttpResponseStatus.CREATED)
    rep.getContentString() must include ("/path/to/file")
  }

  it must "render Action.Moved" in {
    val rep = ActionResultHttpRenderer(Action.Moved(metadata)).build
    rep.status must be (HttpResponseStatus.CREATED)
    rep.getContentString() must include ("/path/to/file")
  }

  it must "render Action.Deleted" in {
    val rep = ActionResultHttpRenderer(Action.Deleted(metadata)).build
    rep.status must be (HttpResponseStatus.NO_CONTENT)
    rep.getContentString() must be ('empty)
  }

  it must "render Action.OwnerSet" in {
    val rep = ActionResultHttpRenderer(Action.OwnerSet(metadata)).build
    rep.status must be (HttpResponseStatus.NO_CONTENT)
    rep.getContentString() must be ('empty)
  }

  it must "render Action.GroupSet" in {
    val rep = ActionResultHttpRenderer(Action.GroupSet(metadata)).build
    rep.status must be (HttpResponseStatus.NO_CONTENT)
    rep.getContentString() must be ('empty)
  }

  it must "render Action.PermissionsSet" in {
    val rep = ActionResultHttpRenderer(Action.PermissionsSet(metadata)).build
    rep.status must be (HttpResponseStatus.NO_CONTENT)
    rep.getContentString() must be ('empty)
  }
}
