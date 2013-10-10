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

import org.scalatest.{FlatSpec, Tag}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.controllers.pages.NamedKey
import es.tid.cosmos.servicemanager.ClusterId

trait CosmosProfileDaoBehavior { this: FlatSpec with MustMatchers =>

  type DaoTest = CosmosProfileDao => Unit

  def profileDao(withDao: DaoTest => Unit, maybeTag: Option[Tag] = None) {

    val unknownCosmosId = 1000

    def taggedTest(subject: ItVerbString, testFun: => Unit) =
      maybeTag.map(tag => subject taggedAs tag in testFun)
        .getOrElse(subject in testFun)

    taggedTest(it must "register new users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val id = UserId("db-0003")
        val profile = dao.registerUserInDatabase(id, Registration("jsmith", "pk00001"))
        profile.handle must be ("jsmith")
        profile.keys.length must be (1)
      }
    })

    taggedTest(it must "change the handle of users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val userId = UserId("db-0003")
        val cosmosId = dao.registerUserInDatabase(userId, Registration("jsmith", "pk00001")).id
        dao.setHandle(cosmosId, "js")
        dao.lookupByUserId(userId).get.handle must equal ("js")
      }
    })

    taggedTest(it must "not change the handle of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setHandle(unknownCosmosId, "js")
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "not change the handle to a repeated one", withDao { dao =>
      dao.withTransaction { implicit c =>
        dao.registerUserInDatabase(UserId("db001"), Registration("existing", "pk00001"))
        val cosmosId =
          dao.registerUserInDatabase(UserId("db002"), Registration("current", "pk00002")).id
        evaluating {
          dao.setHandle(cosmosId, "existing")
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "change the keys of users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val userId = UserId("db-0003")
        val cosmosId = dao.registerUserInDatabase(userId, Registration("jsmith", "pk00001")).id
        val newKeys = Seq(NamedKey("default", "pk2"), NamedKey("extra", "pk3"))
        dao.setPublicKeys(cosmosId, newKeys)
        dao.lookupByUserId(userId).get.keys must equal (newKeys)
      }
    })

    taggedTest(it must "not change the keys of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setPublicKeys(unknownCosmosId, Seq(NamedKey("default", "pk")))
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "detect unused handles", withDao { dao =>
      dao.withTransaction { implicit c =>
        dao.registerUserInDatabase(UserId("oauth53"), Registration("used_handle", "secure_pk"))
        dao.handleExists("used_handle") must be (true)
        dao.handleExists("unused_handle") must be (false)
      }
    })

    taggedTest(it must "get Cosmos ID from user ID when user is registered", withDao { dao =>
      dao.withTransaction { implicit c =>
        val registeredUser = UserId("db-registered")
        dao.registerUserInDatabase(registeredUser, Registration("jsmith", "pk00001"))
        dao.getCosmosId(registeredUser) must be ('defined)
        dao.getCosmosId(UserId("db-unknown")) must not be ('defined)
      }
    })

    taggedTest(it must "get empty machine quota for unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val unknownUserId : Long = 1001
        dao.getMachineQuota(unknownUserId) must equal (EmptyQuota)
      }
    })

    taggedTest(it must "lookup a profile from api credentials", withDao { dao =>
      dao.withTransaction { implicit c =>
        val id = UserId("db-0004")
        dao.registerUserInDatabase(id, Registration("user4", "pk00004"))
        val profileByUserId = dao.lookupByUserId(id).get
        val profileByApiCredentials =
          dao.lookupByApiCredentials(profileByUserId.apiCredentials).get
        profileByUserId must be (profileByApiCredentials)
      }
    })

    taggedTest(it must "assign cluster ownership and remember it", withDao { dao =>
      dao.withTransaction { implicit c =>
        val clusterId = ClusterId()
        val id1 = dao.registerUserInDatabase(UserId("user1"), Registration("user1", "pk0001")).id
        val id2 = dao.registerUserInDatabase(UserId("user2"), Registration("user2", "pk0002")).id
        dao.assignCluster(clusterId, id2)
        dao.clustersOf(id1).map(_.clusterId).toList must not contain clusterId
        dao.clustersOf(id2).map(_.clusterId).toList must contain (clusterId)
      }
    })
  }
}
