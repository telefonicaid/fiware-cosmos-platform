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

package es.tid.cosmos.infinity.server.fs

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.authorization.FilePermissions
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._

class InodeTest extends FlatSpec with MustMatchers {

  val sampleUser = UserProfile("bob", "staff")

  val sampleInode = Inode (
    id = "325124356",
    name = "sample",
    isDirectory = true,
    permissions = FilePermissions("root", "root", fromOctal("777")),
    parentId = "67531465"
  )
  
  "Inode" must "instantiate a new child inode" in {
    val usersDir = sampleInode.newChild("users", isDirectory = true, user = sampleUser)
    assert(usersDir.id != null)
    assert(usersDir.name == "users")
    assert(usersDir.parentId == sampleInode.id)
  }

  it must "reject creation of inode from non-directory inode" in {
    val file = sampleInode.newChild("my-file.txt", isDirectory = false, user = sampleUser)
    evaluating {
      file.newChild("other-file.txt", isDirectory = false, user = sampleUser)
    } must produce[UnsupportedOperationException]
  }
}
