/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
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
      scenario("Initially, the user's directory must be empty") {
        infinity.ls must be ('empty)
      }
      scenario("The user can upload a file to Infinity through the CLI") {
        infinity.put(source, target)
        infinity.ls must be (Stream(target))
      }

      scenario("The user can get the file that was uploaded") {
        infinity.get(target, localCopy).exists must be (true)
      }

      scenario("The user can remove the file from Infinity") {
        infinity.rm(target)
        infinity.ls must be ('empty)
      }
    }
  }
}
