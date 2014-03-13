package es.tid.cosmos.api.controllers.pages

import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.quota.{FiniteQuota, EmptyQuota, GuaranteedGroup}

trait WithSampleGroups extends WithTestApplication {
  val noQuotaGroup = GuaranteedGroup("groupNoQuota", EmptyQuota)
  val groupWithQuota = GuaranteedGroup("group5Quota", FiniteQuota(5))
  store.withTransaction { implicit c =>
    store.group.register(noQuotaGroup)
    store.group.register(groupWithQuota)
  }
}
