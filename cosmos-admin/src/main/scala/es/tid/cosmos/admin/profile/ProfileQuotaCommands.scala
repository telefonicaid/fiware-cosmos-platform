package es.tid.cosmos.admin.profile

import scalaz.Validation

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.api.profile.dao.ProfileDataStore
import es.tid.cosmos.api.quota.{UnlimitedQuota, Quota}

private[profile] class ProfileQuotaCommands(override val store: ProfileDataStore)
  extends ProfileCommands.QuotaCommands with ProfileValidations {

  def set(handle: String, limit: Int): CommandResult = setMachineQuota(handle, Quota(limit))

  def remove(handle: String): CommandResult = setMachineQuota(handle, UnlimitedQuota)

  private def setMachineQuota(handle: String, quota: Quota) =
    CommandResult.fromValidation(store.withTransaction { implicit c =>
      for {
        cosmosProfile <- requireProfileWithHandle(handle)
        _ <- Validation.fromTryCatch(
          store.profile.setMachineQuota(cosmosProfile.id, quota)
        ).leftMap(_.getMessage)
      } yield CommandResult.success(s"Machine quota for user $handle changed to $quota")
    })
}
