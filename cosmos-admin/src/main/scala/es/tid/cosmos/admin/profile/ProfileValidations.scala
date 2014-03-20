package es.tid.cosmos.admin.profile

import scalaz.{Scalaz, Validation}

import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.api.profile.dao.ProfileDataStore

private[profile] trait ProfileValidations {
  import Scalaz._

  val store: ProfileDataStore

  protected def requireProfileWithHandle(
      handle: String)(implicit c: store.Conn): Validation[String, CosmosProfile] =
    store.profile.lookupByHandle(handle).toSuccess(s"No user with handle $handle")
}
