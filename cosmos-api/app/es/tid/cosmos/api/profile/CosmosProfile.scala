package es.tid.cosmos.api.profile

import es.tid.cosmos.api.auth.ApiCredentials

/**
  * Represents the Cosmos-specific user profile.
  *
  * @param id             Internal user id
  * @param handle         User login
  * @param quota          Quota for resources
  * @param apiCredentials Credentials for the REST API
  * @param keys           Public keys
  */
case class CosmosProfile(
    id: Long,
    handle: String,
    group: Group,
    quota: Quota,
    apiCredentials: ApiCredentials,
    keys: Seq[NamedKey]) {

  require(HandleConstraint(handle), s"Invalid handle: $handle")
  require(duplicatedKeys.isEmpty, s"Duplicated keys: ${duplicatedKeys.mkString("", ", ", "")}")
//  require(group.minimumQuota.withinQuota(quota), "User quota exceeds its group quota")

  private def duplicatedKeys = for {
    (name, group) <- keys.groupBy(_.name) if group.size > 1
  } yield name
}
