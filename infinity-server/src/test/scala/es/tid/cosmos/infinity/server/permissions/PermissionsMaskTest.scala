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

package es.tid.cosmos.infinity.server.permissions

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.fs.FilePermissions

class PermissionsMaskTest extends FlatSpec with MustMatchers {

  "Unix file permissions" must "be convertible from valid octal representation" in {
    PermissionsMask.fromOctal("752") must be (PermissionsMask(
      owner = PermissionClass(read = true, write = true, execute = true),
      group = PermissionClass(read = true, write = false, execute = true),
      others = PermissionClass(read = false, write = true, execute = false)
    ))
  }

  it must "print itself as octal value" in {
    PermissionsMask.fromOctal("752").toString must be ("752")
  }

  it must "reject conversion from invalid octal representation" in {
    intercept[IllegalArgumentException] { PermissionsMask.fromOctal("ABC") }
    intercept[IllegalArgumentException] { PermissionsMask.fromOctal("759") }
    intercept[IllegalArgumentException] { PermissionsMask.fromOctal("7553") }
  }

  "File Permissions" must "check permissions correctly" in {
    val filePermission = FilePermissions("bob", "staff", PermissionsMask.fromOctal("754"))

    val bob = UserProfile("bob", "bob")

    filePermission.canRead(bob) must be (true)
    filePermission.canWrite(bob) must be (true)
    filePermission.canExec(bob) must be (true)

    val bobImpersonated = UserProfile("bob", "staff", PermissionsMask.fromOctal("077"))

    filePermission.canRead(bobImpersonated) must be (true)
    filePermission.canWrite(bobImpersonated) must be (false)
    filePermission.canExec(bobImpersonated) must be (true)

    val alice = UserProfile("alice", "staff")

    filePermission.canRead(alice) must be (true)
    filePermission.canWrite(alice) must be (false)
    filePermission.canExec(alice) must be (true)

    val john = UserProfile("john", "other")

    filePermission.canRead(john) must be (true)
    filePermission.canWrite(john) must be (false)
    filePermission.canExec(john) must be (false)

    val superuser = UserProfile("hdfs", "hdfs", superuser = true)

    filePermission.canRead(superuser) must be (true)
    filePermission.canWrite(superuser) must be (true)
    filePermission.canExec(superuser) must be (true)
  }
}
