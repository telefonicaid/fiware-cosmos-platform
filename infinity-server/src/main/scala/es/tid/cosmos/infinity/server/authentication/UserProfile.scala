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

import es.tid.cosmos.infinity.server.permissions.PermissionsMask

/** The profile of a user in Infinity.
  *
  * @param username The name of the user.
  * @param group The group of the user
  * @param mask The mask applied to the UNIX permissions for the user.
  * @param accessFrom The set of hostnames where this profile is allowed to access from.
  *                   An empty set means any host.
  * @param superuser Superuser can modify filesystem without any restrictions
  */
case class UserProfile(
  username: String,
  group: String,
  mask: PermissionsMask = PermissionsMask.fromOctal("777"),
  accessFrom: Set[String] = Set.empty,
  superuser: Boolean = false
)
