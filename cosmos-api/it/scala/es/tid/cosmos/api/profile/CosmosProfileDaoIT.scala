package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.db.DB

import es.tid.cosmos.api.controllers.pages.Registration
import es.tid.cosmos.api.mocks.{WithSampleUsers, WithInMemoryDatabase}
import es.tid.cosmos.servicemanager.ClusterId

class CosmosProfileDaoIT extends FlatSpec with MustMatchers {

  "Cosmos Profile DAO" must "register new users" in new WithInMemoryDatabase {
    DB.withTransaction { implicit c =>
      CosmosProfileDao.registerUserInDatabase("db-0003", Registration("jsmith", "pk00001"))
      val profile = CosmosProfileDao.lookupByUserId("db-0003").get
      profile.handle must be ("jsmith")
      profile.keys.length must be (1)
    }
  }

  it must "get Cosmos ID from user ID when user is registered" in new WithInMemoryDatabase {
    DB.withTransaction { implicit c =>
      CosmosProfileDao.registerUserInDatabase("db-registered", Registration("jsmith", "pk00001"))
      CosmosProfileDao.getCosmosId("db-registered") must be ('defined)
      CosmosProfileDao.getCosmosId("db-unknown") must not be ('defined)
    }
  }

  it must "lookup a profile from api credentials" in new WithInMemoryDatabase {
    DB.withTransaction { implicit c =>
      CosmosProfileDao.registerUserInDatabase("db-0004", Registration("user4", "pk00004"))
      val profileByUserId = CosmosProfileDao.lookupByUserId("db-0004").get
      val profileByApiCredentials =
        CosmosProfileDao.lookupByApiCredentials(profileByUserId.apiCredentials).get
      profileByUserId must be (profileByApiCredentials)
    }
  }

  it must "assign cluster ownership and remember it" in new WithSampleUsers {
    DB.withTransaction { implicit c =>
      val clusterId = ClusterId()
      CosmosProfileDao.assignCluster(clusterId, user2.id)
      CosmosProfileDao.clustersOf(user1.id).toList must not contain (clusterId)
      CosmosProfileDao.clustersOf(user2.id).toList must contain (clusterId)
    }
  }
}
