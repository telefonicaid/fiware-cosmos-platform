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

package es.tid.cosmos.infinity.common

import java.net.InetAddress

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.permissions.{UserProfile, PermissionsMask}

class UserProfileTest extends FlatSpec with MustMatchers {

  val allowedHost = InetAddress.getByName("10.95.236.25")
  val otherHost = InetAddress.getByName("10.95.236.26")

  val userWithoutWhitelist = UserProfile(
    username = "username",
    groups = Seq("group"),
    mask = PermissionsMask.fromOctal("770"),
    accessFrom = None,
    superuser = false
  )
  val userWithWhitelist = userWithoutWhitelist.copy(accessFrom = Some(Set("10.95.236.25")))

  "A user profile with no access whitelist" must "be accessible from anywhere" in {
    userWithoutWhitelist.accessibleFrom(allowedHost) must be (true)
    userWithoutWhitelist.accessibleFrom(otherHost) must be (true)
  }

  "A user profile with access whitelist" must "be accessible from origins on the whitelist" in {
    userWithWhitelist.accessibleFrom(allowedHost) must be (true)
    userWithWhitelist.accessibleFrom(otherHost) must be (false)
  }
}
