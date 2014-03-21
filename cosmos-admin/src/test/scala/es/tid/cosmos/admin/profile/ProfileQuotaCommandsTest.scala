package es.tid.cosmos.admin.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.quota.{UnlimitedQuota, Quota}

class ProfileQuotaCommandsTest extends FlatSpec with MustMatchers {

  trait WithQuotaCommands extends MockCosmosDataStoreComponent {
    val handle = "jsmith"
    val cosmosId = registerUser(handle)(store).id
    val quotaCommands = new ProfileQuotaCommands(store)

    def currentQuota() = store.withTransaction { implicit c =>
      store.profile.lookupByHandle(handle)
    }.get.quota
  }

  "A quota profile command" must "unset quota" in new WithQuotaCommands {
    store.withTransaction { implicit c =>
      store.profile.setMachineQuota(cosmosId, Quota(10))
    }
    quotaCommands.remove(handle) must be ('success)
    currentQuota() must be (UnlimitedQuota)
  }

  it must "set a valid quota" in new WithQuotaCommands {
    quotaCommands.set(handle, 15) must be ('success)
    currentQuota() must be (Quota(15))
  }
}
