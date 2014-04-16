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

package es.tid.cosmos.api.profile

import es.tid.cosmos.api.profile.CosmosProfile._
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.api.quota._

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
    capabilities: UserCapabilities = DefaultUserCapabilities) extends QuotaConsumer[ProfileId] {

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
