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

package es.tid.cosmos.tests.e2e

import java.io.File.createTempFile

class PersistentHdfsAccessIT extends E2ETestBase {
  val source = {
    val file = createTempFile("test", "txt")
    file.deleteOnExit()
    file.getAbsolutePath
  }
  val target = "testInHdfs.txt"
  val localCopy = {
    val file = createTempFile("localcopy", "txt")
    file.delete()
    file.getAbsolutePath
  }

  withNewUser { user =>
    lazy val infinity = new PersistentHdfs(user)

    feature("Users can use Infinity to persist important data") {
      ignore("Initially, the user's directory must be empty") {
        infinity.ls() must be ('empty)
      }

      ignore("The user can upload a file to Infinity through the CLI") {
        infinity.put(source, target)
        infinity.ls() must be (Seq(target))
      }

      ignore("The user can get the file that was uploaded") {
        infinity.get(target, localCopy).exists must be (true)
      }

      ignore("The user can remove the file from Infinity") {
        infinity.rm(target)
        infinity.ls() must be ('empty)
      }
    }
  }
}
