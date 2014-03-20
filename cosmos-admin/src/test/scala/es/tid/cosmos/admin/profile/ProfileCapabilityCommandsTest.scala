package es.tid.cosmos.admin.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.{CapabilityMatchers, Capability}
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent

class ProfileCapabilityCommandsTest extends FlatSpec with MustMatchers with CapabilityMatchers {

  trait WithCapabilityCommands extends MockCosmosDataStoreComponent {
    val handle = "jsmith"
    val cosmosId = registerUser(handle)(store).id
    val capabilityCommands = new ProfileCapabilityCommands(store)

    def currentCapabilities() = store.withTransaction { implicit c =>
      store.profile.lookupByHandle(handle)
    }.get.capabilities
  }

  "A capability command" must "enable user capabilities" in new WithCapabilityCommands {
    capabilityCommands.enable(handle, "is_sudoer") must be ('success)
    currentCapabilities() must containCapability(Capability.IsSudoer)
  }

  it must "disable user capabilities" in new WithCapabilityCommands {
    store.withTransaction { implicit c =>
      store.capability.enable(cosmosId, Capability.IsSudoer)
    }
    capabilityCommands.disable(handle, "is_sudoer") must be ('success)
    currentCapabilities() must not (containCapability(Capability.IsSudoer))
  }
}
