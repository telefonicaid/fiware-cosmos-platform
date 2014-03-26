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

package es.tid.cosmos.infinity.server.authentication

import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions

/** The profile of a user in Infinity.
  *
  * @param username The name of the user.
  * @param group The group of the user
  * @param unixPermissionMask The mask applied to the UNIX permissions for the user.
  * @param accessFrom The set of hostnames where this profile is allowed to access from.
  *                   An empty set means any host.
  */
case class UserProfile(
  username: String,
  group: String,
  unixPermissionMask: UnixFilePermissions,
  accessFrom: Set[String] = Set.empty
)
