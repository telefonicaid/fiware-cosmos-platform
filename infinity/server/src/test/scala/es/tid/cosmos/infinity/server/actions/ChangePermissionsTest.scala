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

class ChangePermissionsTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Change permissions action" must "return PermissionsSet upon successful permissions set" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).setPermissions(on, newMask)
    doReturn(Future.successful(metadata)).when(nameNode).pathMetadata(on)
    changePermissions(context) must eventually (be (MetadataAction.PermissionsSet(metadata)))
  }

  it must "fail if name node fails to set permissions" in new Fixture {
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot change permissions"))))
      .when(nameNode).setPermissions(on, newMask)
    changePermissions(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  it must "fail if name node fails to retrieve new metadata" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).setPermissions(on, newMask)
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot retrieve metadata"))))
      .when(nameNode).pathMetadata(on)
    changePermissions(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  trait Fixture extends ActionFixture {
    val newMask = PermissionsMask.fromOctal("640")
    val changePermissions = ChangePermissions(nameNode, on, newMask)
  }
}
