package es.tid.cosmos.admin.profile

import scalaz.{Scalaz, Validation}

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.api.profile.dao.{CapabilityDataStore, ProfileDataStore}
import es.tid.cosmos.api.profile.Capability
import es.tid.cosmos.api.profile.Capability.Capability

private[profile] class ProfileCapabilityCommands(
    override val store: ProfileDataStore with CapabilityDataStore)
  extends ProfileCommands.CapabilityCommands with ProfileValidations {

  import Scalaz._

  override def enable(handle: String, capability: String): CommandResult =
    modifyCapability(handle, capability, enable = true)

  override def disable(handle: String, capability: String): CommandResult =
    modifyCapability(handle, capability, enable = false)

  private def modifyCapability(handle: String, capability: String, enable: Boolean): CommandResult =
    CommandResult.fromValidation(store.withTransaction { implicit c =>
      val (verb, action) =
        if (enable) ("enabled", store.capability.enable _)
        else ("disabled", store.capability.disable _)
      for {
        cosmosProfile <- requireProfileWithHandle(handle)
        parsedCapability <- requireValidCapability(capability)
      } yield {
        action(cosmosProfile.id, parsedCapability)
        CommandResult.success(s"$parsedCapability $verb for user $handle")
      }
    })

  private def requireValidCapability(input: String): Validation[String, Capability] =
    Capability.values.find(_.toString == input).toSuccess(
      s"Unknown capability '$input', one of ${Capability.values.mkString(", ")} was expected"
    )
}
