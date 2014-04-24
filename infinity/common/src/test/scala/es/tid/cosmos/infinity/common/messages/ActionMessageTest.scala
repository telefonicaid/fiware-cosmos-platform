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

package es.tid.cosmos.infinity.common.messages

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.RootPath
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class ActionMessageTest extends FlatSpec with MustMatchers {

  val validCreateFile = CreateFile(
    name = "foo.txt",
    permissions = PermissionsMask.fromOctal("640"),
    replication = Some(3),
    blockSize = None
  )

  "CreateFile message" must "require a valid name" in {
    evaluating {
      validCreateFile.copy(name = "invalid/path/element")
    } must produce [IllegalArgumentException]
  }

  it must "require strictly positive replication factor" in {
    evaluating {
      validCreateFile.copy(replication = Some(-1))
    } must produce [IllegalArgumentException]
  }

  it must "require strictly positive block size" in {
    evaluating {
      validCreateFile.copy(blockSize = Some(0))
    } must produce [IllegalArgumentException]
  }

  "CreateDirectory message" must "require a valid name" in {
    evaluating {
      CreateDirectory(name = "invalid/path/element", permissions = PermissionsMask.fromOctal("640"))
    } must produce [IllegalArgumentException]
  }

  "Move message" must "require a valid name" in {
    evaluating {
      Move(name = "invalid/path/element", from = RootPath)
    } must produce [IllegalArgumentException]
  }
}
