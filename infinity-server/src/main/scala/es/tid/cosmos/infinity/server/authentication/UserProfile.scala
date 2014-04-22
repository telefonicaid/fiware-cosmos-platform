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

package es.tid.cosmos.infinity.server.authentication

import java.net.{UnknownHostException, InetAddress}

import org.apache.commons.logging.LogFactory

import es.tid.cosmos.infinity.server.permissions.PermissionsMask

/** The profile of a user in Infinity.
  *
  * @param username The name of the user.
  * @param group The group of the user
  * @param mask The mask applied to the UNIX permissions for the user.
  * @param accessFrom The set of hostnames where this profile is allowed to access from.
  *                   None means that can be accessed from any host but Some(Set.empty)
  *                   means that can be use from no host..
  * @param superuser Superuser can modify filesystem without any restrictions
  */
case class UserProfile(
  username: String,
  group: String,
  mask: PermissionsMask = PermissionsMask.fromOctal("777"),
  accessFrom: Option[Set[String]] = None,
  superuser: Boolean = false
) {

  def accessibleFrom(origin: InetAddress): Boolean =
    accessFrom.isEmpty || whitelistAddresses().contains(origin)

  /** Resolve addresses of the whitelist. */
  private def whitelistAddresses(): Set[InetAddress] =
    accessFrom.getOrElse(Set.empty).flatMap(resolveOrigin)

  private def resolveOrigin(origin: String): Set[InetAddress] = try {
    InetAddress.getAllByName(origin).toSet
  } catch {
    case ex: UnknownHostException =>
      UserProfile.Log.warn(
        s"Cannot resolve '$origin' as part of the whitelist of user profile $this", ex)
      Set.empty
  }
}

object UserProfile {
  private val Log = LogFactory.getLog(classOf[UserProfile])
}
