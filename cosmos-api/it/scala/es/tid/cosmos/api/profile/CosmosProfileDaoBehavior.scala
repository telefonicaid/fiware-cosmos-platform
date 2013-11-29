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

import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait CosmosProfileDaoBehavior extends CapabilityMatchers { this: FlatSpec with MustMatchers =>

  type DaoTest = CosmosProfileDao => Unit

  def profileDao(withDao: DaoTest => Unit, maybeTag: Option[Tag] = None) {

    val unknownCosmosId = 1000

    def taggedTest(subject: ItVerbString, testFun: => Unit) =
      maybeTag.map(tag => subject taggedAs tag in testFun)
        .getOrElse(subject in testFun)

    taggedTest(it must "register new users", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val profile = registerUser(dao, "jsmith")
        profile.handle must be ("jsmith")
        profile.keys.length must be (1)
        profile.email must be ("jsmith@example.com")
        profile.keys must be (Seq(NamedKey("default", s"ssh-rsa AAAAAA jsmith@example.com")))
        profile.quota must be (CosmosProfile.DefaultQuota)
        profile.group must be (CosmosProfile.DefaultGroup)
        profile.capabilities must be (CosmosProfile.DefaultUserCapabilities)
      }
    })

    taggedTest(it must "list all users", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        registerUser(dao, "user1")
        registerUser(dao, "user2")
        dao.getAllUsers().map(_.handle) must be (Seq("user1", "user2"))
      }
    })

    taggedTest(it must "change the handle of users", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        dao.setHandle(cosmosId, "jsm")
        dao.lookupByUserId(userIdFor("jsmith")).get.handle must equal ("jsm")
      }
    })

    taggedTest(it must "not change the handle of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setHandle(unknownCosmosId, "jsm")
        } must produce [CosmosProfileException]
      }
    })

    taggedTest(it must "change the email of users", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        dao.setEmail(cosmosId, "new@mail.com")
        dao.lookupByUserId(userIdFor("jsmith")).get.email must equal ("new@mail.com")
      }
    })

    taggedTest(it must "not change the email of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setEmail(unknownCosmosId, "new@mail.com")
        } must produce [CosmosProfileException]
      }
    })

    taggedTest(it must "change the state of users", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        dao.setUserState(cosmosId, Deleting)
        dao.lookupByUserId(userIdFor("jsmith")).get.state must equal (Deleting)
      }
    })

    taggedTest(it must "not change the state of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setUserState(unknownCosmosId, Disabled)
        } must produce [CosmosProfileException]
      }
    })

    taggedTest(it must "not change the handle to a repeated one", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        registerUser("existing")
        val cosmosId = registerUser(dao, "current").id
        evaluating {
          dao.setHandle(cosmosId, "existing")
        } must produce [CosmosProfileException]
      }
    })

    taggedTest(it must "change the keys of users", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        val newKeys = Seq(
          NamedKey("default", "ssh-rsa AAAAA jsmith@host"),
          NamedKey("extra", "ssh-rsa BBBBB jsmith@host"))
        dao.setPublicKeys(cosmosId, newKeys)
        dao.lookupByUserId(userIdFor("jsmith")).get.keys must equal (newKeys)
      }
    })

    taggedTest(it must "not change the keys of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setPublicKeys(unknownCosmosId, Seq(NamedKey("default", "ssh-rsa AAAAA jsmith@host")))
        } must produce [CosmosProfileException]
      }
    })

    taggedTest(it must "set user default capabilities to unstrusted", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val userProfile = registerUser(dao, "jsmith")

        userProfile.capabilities must be (UntrustedUserCapabilities)
      }
    })

    taggedTest(it must "enable user capabilities", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        registerUser(dao, "jsmith")
        val userProfile2 = registerUser(dao, "bclinton")
        dao.enableUserCapability(userProfile2.id, Capability.IsSudoer)
        lookup(dao, "jsmith").get must not(haveCapability (Capability.IsSudoer))
        lookup(dao, "bclinton").get must haveCapability (Capability.IsSudoer)
      }
    })

    taggedTest(it must "disable user capabilities", withDao { implicit dao =>
      registerUser("jsmith")
      val userProfile2 = registerUser("bclinton")
      dao.withTransaction { implicit c =>
        dao.enableUserCapability(userProfile2.id, Capability.IsSudoer)
        dao.disableUserCapability(userProfile2.id, Capability.IsSudoer)
        lookup(dao, "jsmith").get must not(haveCapability(Capability.IsSudoer))
        lookup(dao, "bclinton").get must not(haveCapability(Capability.IsSudoer))
      }
    })

    taggedTest(it must "detect unused handles", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        registerUser(dao, "usedHandle")
        dao.handleExists("usedHandle") must be (true)
        dao.handleExists("unusedHandle") must be (false)
      }
    })

    taggedTest(it must "get Cosmos ID from user ID when user is registered", withDao {
      implicit dao =>
        dao.withTransaction { implicit c =>
          registerUser(dao, "registered")
          dao.getProfileId(userIdFor("registered")) must be ('defined)
          dao.getProfileId(UserId("db-unknown")) must not be 'defined
        }
    })

    taggedTest(it must "get empty machine quota for unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val unknownUserId : Long = 1001
        dao.getMachineQuota(unknownUserId) must equal (EmptyQuota)
      }
    })
    
    taggedTest(it must "set machine quota for a given user", withDao { dao =>
      dao.withTransaction { implicit c =>
        val id1 = registerUser(dao, "jsmith").id
        val id2 = registerUser(dao, "bclinton").id
        val id3 = registerUser(dao, "imontoya").id
        val id4 = registerUser(dao, "vizzini").id

        dao.getMachineQuota(id1) must be (CosmosProfile.DefaultQuota)
        dao.getMachineQuota(id2) must be (CosmosProfile.DefaultQuota)
        dao.getMachineQuota(id3) must be (CosmosProfile.DefaultQuota)
        dao.getMachineQuota(id4) must be (CosmosProfile.DefaultQuota)

        dao.setMachineQuota(id2, FiniteQuota(7))
        dao.setMachineQuota(id3, UnlimitedQuota)
        dao.setMachineQuota(id4, EmptyQuota)

        dao.getMachineQuota(id1) must be (CosmosProfile.DefaultQuota)
        dao.getMachineQuota(id2) must be (FiniteQuota(7))
        dao.getMachineQuota(id3) must be (UnlimitedQuota)
        dao.getMachineQuota(id4) must be (EmptyQuota)
      }
    })

    taggedTest(it must "lookup a profile from api credentials", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        registerUser(dao, "user4")
        val profileByUserId = dao.lookupByUserId(userIdFor("user4")).get
        val profileByApiCredentials =
          dao.lookupByApiCredentials(profileByUserId.apiCredentials).get
        profileByUserId must be (profileByApiCredentials)
      }
    })

    taggedTest(it must "lookup existing profiles by handle", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val profile = registerUser(dao, "handle")
        dao.lookupByHandle("handle") must be (Some(profile))
        dao.lookupByHandle("unknown") must not be 'defined
      }
    })

    taggedTest(it must "assign cluster ownership and remember it", withDao { implicit dao =>
      dao.withTransaction { implicit c =>
        val clusterId = ClusterId()
        val id1 = registerUser(dao, "user1").id
        val id2 = registerUser(dao, "user2").id
        dao.assignCluster(clusterId, id2)
        dao.clustersOf(id1).map(_.clusterId).toList must not contain clusterId
        dao.clustersOf(id2).map(_.clusterId).toList must contain (clusterId)
      }
    })

    taggedTest(it must "retrieve the owner of a cluster", withDao { implicit dao =>
      val clusterId = ClusterId()
      val profileId = dao.withTransaction { implicit c =>
        val profileId = registerUser(dao, "user1").id
        dao.assignCluster(clusterId, profileId)
        profileId
      }
      dao.withTransaction { implicit c =>
        dao.ownerOf(clusterId) must be (Some(profileId))
      }
    })

    taggedTest(it must "get empty, NoGroup by default", withDao{ dao =>
      dao.withConnection{ implicit c =>
        dao.getGroups must equal (Set(NoGroup))
      }
    })

    taggedTest(it must "get all registered groups + NoGroup", withDao{ dao =>
      val (a, b, c) = (
        GuaranteedGroup("A", EmptyQuota),
        GuaranteedGroup("B", Quota(2)),
        GuaranteedGroup("C", Quota(3))
      )
      dao.withTransaction{ implicit conn =>
        Seq(a, b, c).foreach(dao.registerGroup)
        dao.getGroups must be (Set(a, b, c, NoGroup))
      }
    })

    taggedTest(it must "set a user's group to an existing one", withDao { implicit dao =>
      val group = GuaranteedGroup("A", Quota(7))
      dao.withTransaction{ implicit c =>
        dao.registerGroup(group)
        val id1 = registerUser("user1").id
        registerUser("user2")
        dao.lookupByUserId(userIdFor("user1")).get.group must be (NoGroup)
        dao.lookupByUserId(userIdFor("user2")).get.group must be (NoGroup)
        dao.setGroup(id1, Some(group.name))
        dao.lookupByUserId(userIdFor("user1")).get.group must be (group)
        dao.lookupByUserId(userIdFor("user2")).get.group must be (NoGroup)
      }
    })

    taggedTest(it must "delete an existing group", withDao { implicit dao =>
      val group: Group = GuaranteedGroup("A", Quota(7))
      dao.withTransaction{ implicit c =>
        dao.registerGroup(group)
        val id = registerUser("user1").id
        dao.setGroup(id, Some(group.name))
        dao.deleteGroup(group.name)
        dao.getGroups must not (contain (group))
        dao.lookupByUserId(userIdFor("user1")).get.group must be (NoGroup)
      }
    })

    taggedTest(it must "set the minimum quota of an existing group", withDao { implicit dao =>
      val group = GuaranteedGroup("A", Quota(7))
      dao.withTransaction { implicit c =>
        dao.registerGroup(group)
        val id = registerUser("user1").id
        dao.setGroup(id, Some(group.name))
        dao.setGroupQuota(group.name, EmptyQuota)
        dao.getGroups.find(_.name == group.name).get.minimumQuota must be (EmptyQuota)
        dao.lookupByUserId(userIdFor("user1"))
          .get.group must be (GuaranteedGroup(group.name, EmptyQuota))
      }
    })

    taggedTest(it must "lookup users by group", withDao{ implicit dao =>
      dao.withTransaction { implicit c =>
        val group = GuaranteedGroup("A", Quota(3))
        dao.registerGroup(group)
        val id1 = registerUser("user1").id
        val id2 = registerUser("user2").id
        dao.setGroup(id2, Some(group.name))
        val noGroupProfiles = dao.lookupByGroup(NoGroup)
        val groupProfiles = dao.lookupByGroup(group)
        noGroupProfiles must have size 1
        noGroupProfiles.head.id must equal(id1)
        groupProfiles must have size 1
        groupProfiles.head.id must equal(id2)
      }
    })
  }
}
