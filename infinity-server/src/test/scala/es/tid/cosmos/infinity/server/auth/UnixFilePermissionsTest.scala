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

package es.tid.cosmos.infinity.server.auth

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class UnixFilePermissionsTest extends FlatSpec with MustMatchers {

  "Unix file permissions" must "be convertible from valid octal representation" in {
    UnixFilePermissions.fromOctal("752") must be (UnixFilePermissions(
      owner = PermissionClass(read = true, write = true, execute = true),
      group = PermissionClass(read = true, write = false, execute = true),
      others = PermissionClass(read = false, write = true, execute = false)
    ))
  }

  it must "reject conversion from invalid octal representation" in {
    intercept[IllegalArgumentException] { UnixFilePermissions.fromOctal("ABC") }
    intercept[IllegalArgumentException] { UnixFilePermissions.fromOctal("759") }
    intercept[IllegalArgumentException] { UnixFilePermissions.fromOctal("7553") }
  }

}
