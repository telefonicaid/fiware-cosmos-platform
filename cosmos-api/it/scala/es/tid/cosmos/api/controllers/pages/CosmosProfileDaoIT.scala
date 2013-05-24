package es.tid.cosmos.api.controllers.pages

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.db.DB
import play.api.test.{FakeApplication, WithApplication}
import play.api.test.Helpers._

class CosmosProfileDaoIT extends FlatSpec with MustMatchers {

  class WithInMemoryDatabase
    extends WithApplication(FakeApplication(additionalConfiguration = inMemoryDatabase()))

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
}
