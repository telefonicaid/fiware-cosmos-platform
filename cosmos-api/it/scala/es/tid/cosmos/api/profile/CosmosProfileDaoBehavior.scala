/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.controllers.pages.Registration
import es.tid.cosmos.servicemanager.ClusterId

trait CosmosProfileDaoBehavior { this: FlatSpec with MustMatchers =>

  def profileDao(dao: CosmosProfileDao) {
    it must "register new users" in new WithInMemoryDatabase {
      dao.withTransaction { implicit c =>
        dao.registerUserInDatabase("db-0003", Registration("jsmith", "pk00001"))
        val profile = dao.lookupByUserId("db-0003").get
        profile.handle must be ("jsmith")
        profile.keys.length must be (1)
      }
    }

    it must "get Cosmos ID from user ID when user is registered" in new WithInMemoryDatabase {
      dao.withTransaction { implicit c =>
        dao.registerUserInDatabase("db-registered", Registration("jsmith", "pk00001"))
        dao.getCosmosId("db-registered") must be ('defined)
        dao.getCosmosId("db-unknown") must not be ('defined)
      }
    }

    it must "get empty machine quota for unknown users" in new WithInMemoryDatabase {
      dao.withTransaction { implicit c =>
        val unknownUserId : Long = 1001
        dao.getMachineQuota(unknownUserId) must equal (EmptyQuota)
      }
    }

    it must "lookup a profile from api credentials" in new WithInMemoryDatabase {
      dao.withTransaction { implicit c =>
        dao.registerUserInDatabase("db-0004", Registration("user4", "pk00004"))
        val profileByUserId = dao.lookupByUserId("db-0004").get
        val profileByApiCredentials =
          dao.lookupByApiCredentials(profileByUserId.apiCredentials).get
        profileByUserId must be (profileByApiCredentials)
      }
    }

    it must "assign cluster ownership and remember it" in new WithInMemoryDatabase {
      dao.withTransaction { implicit c =>
        val clusterId = ClusterId()
        val id1 = dao.registerUserInDatabase("user1", Registration("user1", "pk0001"))
        val id2 = dao.registerUserInDatabase("user2", Registration("user2", "pk0002"))
        dao.assignCluster(clusterId, id2)
        dao.clustersOf(id1).toList must not contain (clusterId)
        dao.clustersOf(id2).toList must contain (clusterId)
      }
    }
  }
}
