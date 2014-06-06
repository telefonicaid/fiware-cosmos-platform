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

import java.net.{InetAddress, UnknownHostException}

import org.apache.commons.logging.LogFactory

/** The profile of a user in Infinity.
  *
  * @param username The name of the user.
  * @param groups The groups of the user
  * @param sharedCluster Whether the profile is being accessed from a shared cluster or not
  * @param accessFrom The set of hostnames where this profile is allowed to access from.
  *                   None means that can be accessed from any host but Some(Set.empty)
  *                   means that can be use from no host..
  */
case class UserProfile(
  username: String,
  groups: Seq[String],
  sharedCluster: Boolean = false,
  accessFrom: Option[Set[String]] = None
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
