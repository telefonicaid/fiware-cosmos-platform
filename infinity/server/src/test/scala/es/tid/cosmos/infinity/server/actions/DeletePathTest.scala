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

package es.tid.cosmos.infinity.server.actions

import scala.concurrent.Future

import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.hadoop.NameNodeException

class DeletePathTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Delete path action" must "return Deleted upon successful delete" in new Fixture {
    doReturn(Future.successful(metadata)).when(nameNode).pathMetadata(on)
    doReturn(Future.successful(())).when(nameNode).deletePath(on, isRecursive)
    deletePath(context) must eventually (be (MetadataAction.Deleted(metadata)))
  }

  it must "fail if name node fails to delete the file" in new Fixture {
    doReturn(Future.successful(metadata)).when(nameNode).pathMetadata(on)
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot delete"))))
      .when(nameNode).deletePath(on, isRecursive)
    deletePath(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  it must "fail if name node fails to retrieve old metadata" in new Fixture {
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot retrieve metadata"))))
      .when(nameNode).pathMetadata(on)
    deletePath(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  trait Fixture extends MetadataActionFixture {
    val permissions = PermissionsMask.fromOctal("640")
    val isRecursive = false
    val deletePath = DeletePath(nameNode, on, isRecursive)
  }
}
