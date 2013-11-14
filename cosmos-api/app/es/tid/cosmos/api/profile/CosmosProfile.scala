package es.tid.cosmos.api.profile

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState.UserState

/**
 * Represents the Cosmos-specific user profile.
 *
 * @param id             Internal user id
 * @param state          User state
 * @param handle         User login
 * @param email          User email
 * @param group          The group the user belongs to
 * @param quota          Quota for resources
 * @param apiCredentials Credentials for the REST API
 * @param keys           Public keys
 */
case class CosmosProfile(
    id: Long,
    state: UserState,
    handle: String,
    email: String,
    group: Group,
    quota: Quota,
    apiCredentials: ApiCredentials,
    keys: Seq[NamedKey]) {

  require(HandleConstraint(handle), s"Invalid handle: $handle")
  require(duplicatedKeys.isEmpty, s"Duplicated keys: ${duplicatedKeys.mkString("", ", ", "")}")

  private def duplicatedKeys = for {
    (name, group) <- keys.groupBy(_.name) if group.size > 1
  } yield name
}
