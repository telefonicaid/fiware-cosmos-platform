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

import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.hadoop.HdfsException

class CreateDirectoryTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Create directory action" must "return Created upon successful creation" in new Fixture {
    doNothing().when(nameNode).createDirectory(on, user.username, user.groups.head, permissions)
    doReturn(metadata).when(nameNode).pathMetadata(on)
    createDirectory(context) must eventually (be (MetadataAction.Created(metadata)))
  }

  it must "fail if name node fails to create the directory" in new Fixture {
    doThrow(new HdfsException.IOError(new Exception("cannot change permissions")))
      .when(nameNode).createDirectory(on, user.username, user.groups.head, permissions)
    createDirectory(context) must eventuallyFailWith[HdfsException.IOError]
  }

  it must "fail if name node fails to retrieve new metadata" in new Fixture {
    doNothing().when(nameNode).createDirectory(on, user.username, user.groups.head, permissions)
    doThrow(new HdfsException.IOError(new Exception("cannot create directory")))
      .when(nameNode).pathMetadata(on)
    createDirectory(context) must eventuallyFailWith[HdfsException.IOError]
  }

  trait Fixture extends MetadataActionFixture {
    val permissions = PermissionsMask.fromOctal("640")
    val createDirectory = CreateDirectory(nameNode, on, permissions)
  }
}
