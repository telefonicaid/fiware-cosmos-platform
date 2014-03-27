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

import es.tid.cosmos.infinity.server.authorization.{FilePermissions, UnixFilePermissions}

class InodeTest extends FlatSpec with MustMatchers with SomeUserProfiles {

  "Inode" must "allow creation of directories from root" in {
    val usersDir = RootInode.create("users", true, hdfs)
    assert(usersDir.id != null)
    assert(usersDir.name == "users")
    assert(usersDir.parentId == RootInode.id)
  }

  it must "reject creation of inode from non-directory inode" in {
    val file = RootInode.create("my-file.txt", false, bob)
    intercept[IllegalArgumentException] {
      file.create("other-file.txt", false, bob)
    }
  }

  it must "reject creation of inode with no write permissions in parent" in {
    val bobHome = RootInode.create("bobhome", true, bob,
      FilePermissions("bob", "staff", UnixFilePermissions.fromOctal("755")))
    intercept[IllegalArgumentException] {
      bobHome.create("other-file.txt", false, alice)
    }
  }

  it must "allow renaming with super-user" in {
    val bobHome = RootInode.create("bobhome", true, bob,
      FilePermissions("bob", "staff", UnixFilePermissions.fromOctal("700")))
    val newBob = bobHome.update(name = Some("newbob"), user = hdfs)
    newBob.name must be ("newbob")
  }

}
