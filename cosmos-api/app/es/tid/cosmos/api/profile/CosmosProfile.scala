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

package es.tid.cosmos.api.profile

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.CosmosProfile._
import es.tid.cosmos.api.profile.UserState.UserState

/**
 * Represents the Cosmos-specific user profile.
 *
 * @param id             Internal user id
 * @param state          User state
 * @param handle         User login
 * @param email          User email
 * @param apiCredentials Credentials for the REST API
 * @param keys           Public keys
 * @param group          The group the user belongs to
 * @param quota          Quota for resources
 * @param capabilities   The capabilities of the user
 */
case class CosmosProfile(
    id: Long,
    state: UserState,
    handle: String,
    email: String,
    apiCredentials: ApiCredentials,
    keys: Seq[NamedKey],
    group: Group = DefaultGroup,
    quota: Quota = DefaultQuota,
    capabilities: UserCapabilities = DefaultUserCapabilities) {

  require(HandleConstraint(handle), s"Invalid handle: $handle")
  require(duplicatedKeys.isEmpty, s"Duplicated keys: ${duplicatedKeys.mkString("", ", ", "")}")

  private def duplicatedKeys = for {
    (name, group) <- keys.groupBy(_.name) if group.size > 1
  } yield name
}

object CosmosProfile {
  val DefaultQuota: Quota = FiniteQuota(6)
  val DefaultGroup: Group = NoGroup
  val DefaultUserCapabilities: UserCapabilities = UntrustedUserCapabilities
}
