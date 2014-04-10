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

import es.tid.cosmos.infinity.server.permissions.PermissionsMask

class InodeTest extends FlatSpec with MustMatchers {

  val samplePermissions = FilePermissions("root", "root", PermissionsMask.fromOctal("777"))

  "Child inode" must "require not to be its own parent" in {
    evaluating {
      FileInode("id", "id", "name", samplePermissions)
    } must produce [IllegalArgumentException]
  }

  it must "require not to contain slashes in its name" in {
    evaluating {
      FileInode("id", "parentId", "/name", samplePermissions)
    } must produce [IllegalArgumentException]
  }
}
