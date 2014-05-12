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

class ChangeOwnerTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Change owner action" must "return OwnerSet upon successful owner set" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).setOwner(on, newOwner)
    doReturn(Future.successful(metadata)).when(nameNode).pathMetadata(on)
    changeOwner(context) must eventually (be (Action.OwnerSet(metadata)))
  }

  it must "fail if name node fails to set owner" in new Fixture {
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot change owner"))))
      .when(nameNode).setOwner(on, newOwner)
    changeOwner(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  it must "fail if name node fails to retrieve new metadata" in new Fixture {
    doReturn(Future.successful(())).when(nameNode).setOwner(on, newOwner)
    doReturn(Future.failed(new NameNodeException.IOError(new Exception("cannot retrieve metadata"))))
      .when(nameNode).pathMetadata(on)
    changeOwner(context) must eventuallyFailWith[NameNodeException.IOError]
  }

  trait Fixture extends ActionFixture {
    val newOwner = "saruman"
    val changeOwner = ChangeOwner(nameNode, on, newOwner)
  }
}
