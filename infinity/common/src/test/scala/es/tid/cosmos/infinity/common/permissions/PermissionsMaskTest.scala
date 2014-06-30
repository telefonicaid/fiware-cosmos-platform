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

package es.tid.cosmos.infinity.common.permissions

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class PermissionsMaskTest extends FlatSpec with MustMatchers {

  "Unix file permissions" must "be convertible from valid octal representation" in {
    PermissionsMask.fromOctal("752") must be (PermissionsMask(
      owner = PermissionClass(read = true, write = true, execute = true),
      group = PermissionClass(read = true, write = false, execute = true),
      others = PermissionClass(read = false, write = true, execute = false)
    ))
  }

  it must "be convertible from valid octal representation with sticky bit" in {
    PermissionsMask.fromOctal("1752") must be ('sticky)
  }

  it must "be convertible from a valid short representation" in {
    PermissionsMask.fromShort(0x1ea) must be (PermissionsMask(
      owner = PermissionClass(read = true, write = true, execute = true),
      group = PermissionClass(read = true, write = false, execute = true),
      others = PermissionClass(read = false, write = true, execute = false)
    ))
  }

  it must "be convertible from a valid short representation with sticky bit" in {
    PermissionsMask.fromShort(0x3ea) must be (PermissionsMask(
      owner = PermissionClass(read = true, write = true, execute = true),
      group = PermissionClass(read = true, write = false, execute = true),
      others = PermissionClass(read = false, write = true, execute = false),
      isSticky = true
    ))
  }

  it must "return its short integer representation" in {
    PermissionsMask.fromOctal("752").toShort must be (0x1ea)
  }

  it must "return its short integer representation with sticky bit" in {
    PermissionsMask.fromOctal("1752").toShort must be (0x3ea)
  }

  it must "print itself as octal value" in {
    PermissionsMask.fromOctal("752").toString must be ("752")
    PermissionsMask.fromOctal("0752").toString must be ("752")
    PermissionsMask.fromOctal("1752").toString must be ("1752")
  }

  it must "reject conversion from invalid octal representation" in {
    intercept[IllegalArgumentException] { PermissionsMask.fromOctal("ABC") }
    intercept[IllegalArgumentException] { PermissionsMask.fromOctal("759") }
    intercept[IllegalArgumentException] { PermissionsMask.fromOctal("7553") }
  }
}
