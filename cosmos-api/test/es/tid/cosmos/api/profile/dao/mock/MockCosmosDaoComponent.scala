package es.tid.cosmos.api.profile.dao.mock

import es.tid.cosmos.api.profile.dao.{CosmosProfileDaoComponent, CosmosDao}

trait MockCosmosDaoComponent extends CosmosProfileDaoComponent {
  lazy val cosmosProfileDao: CosmosDao = new MockCosmosDao
}
