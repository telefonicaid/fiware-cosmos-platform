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
import es.tid.cosmos.api.profile.dao.{CosmosDaoException, CosmosDao}
import es.tid.cosmos.api.quota._
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait CosmosDaoBehavior extends CapabilityMatchers { this: FlatSpec with MustMatchers =>

  type DaoTest = CosmosDao => Unit

  def profileDao(withDao: DaoTest => Unit, maybeTag: Option[Tag] = None) {

    val unknownCosmosId = 1000

    def taggedTest(subject: ItVerbString, testFun: => Unit) =
      maybeTag.map(tag => subject taggedAs tag in testFun)
        .getOrElse(subject in testFun)

    taggedTest(it must "register new users", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
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
      dao.store.withTransaction { implicit c =>
        registerUser(dao, "user1")
        registerUser(dao, "user2")
        dao.profile.list().map(_.handle) must be (Seq("user1", "user2"))
      }
    })

    taggedTest(it must "change the handle of users", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        dao.profile.setHandle(cosmosId, "jsm")
        dao.profile.lookupByUserId(userIdFor("jsmith")).get.handle must equal ("jsm")
      }
    })

    taggedTest(it must "not change the handle of unknown users", withDao { dao =>
      dao.store.withTransaction { implicit c =>
        evaluating {
          dao.profile.setHandle(unknownCosmosId, "jsm")
        } must produce [CosmosDaoException]
      }
    })

    taggedTest(it must "change the email of users", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        dao.profile.setEmail(cosmosId, "new@mail.com")
        dao.profile.lookupByUserId(userIdFor("jsmith")).get.email must equal ("new@mail.com")
      }
    })

    taggedTest(it must "not change the email of unknown users", withDao { dao =>
      dao.store.withTransaction { implicit c =>
        evaluating {
          dao.profile.setEmail(unknownCosmosId, "new@mail.com")
        } must produce [CosmosDaoException]
      }
    })

    taggedTest(it must "change the state of users", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        dao.profile.setUserState(cosmosId, Deleting)
        dao.profile.lookupByUserId(userIdFor("jsmith")).get.state must equal (Deleting)
      }
    })

    taggedTest(it must "not change the state of unknown users", withDao { dao =>
      dao.store.withTransaction { implicit c =>
        evaluating {
          dao.profile.setUserState(unknownCosmosId, Disabled)
        } must produce [CosmosDaoException]
      }
    })

    taggedTest(it must "not change the handle to a repeated one", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        registerUser("existing")
        val cosmosId = registerUser(dao, "current").id
        evaluating {
          dao.profile.setHandle(cosmosId, "existing")
        } must produce [CosmosDaoException]
      }
    })

    taggedTest(it must "change the keys of users", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val cosmosId = registerUser(dao, "jsmith").id
        val newKeys = Seq(
          NamedKey("default", "ssh-rsa AAAAA jsmith@host"),
          NamedKey("extra", "ssh-rsa BBBBB jsmith@host"))
        dao.profile.setPublicKeys(cosmosId, newKeys)
        dao.profile.lookupByUserId(userIdFor("jsmith")).get.keys must equal (newKeys)
      }
    })

    taggedTest(it must "not change the keys of unknown users", withDao { dao =>
      dao.store.withTransaction { implicit c =>
        evaluating {
          dao.profile.setPublicKeys(
            unknownCosmosId, Seq(NamedKey("default", "ssh-rsa AAAAA jsmith@host")))
        } must produce [CosmosDaoException]
      }
    })

    taggedTest(it must "set user default capabilities to unstrusted", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val userProfile = registerUser(dao, "jsmith")

        userProfile.capabilities must be (UntrustedUserCapabilities)
      }
    })

    taggedTest(it must "enable user capabilities", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val jsmith = registerUser(dao, "jsmith").id
        val bclinton = registerUser(dao, "bclinton").id

        dao.capability.enable(bclinton, Capability.IsSudoer)

        dao.capability.userCapabilities(jsmith) must not(containCapability(Capability.IsSudoer))
        dao.capability.userCapabilities(bclinton) must containCapability(Capability.IsSudoer)
      }
    })

    taggedTest(it must "disable user capabilities", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val jsmith = registerUser(dao, "jsmith").id
        val bclinton = registerUser(dao, "bclinton").id

        dao.capability.enable(bclinton, Capability.IsSudoer)
        dao.capability.disable(bclinton, Capability.IsSudoer)

        dao.capability.userCapabilities(jsmith) must not(containCapability(Capability.IsSudoer))
        dao.capability.userCapabilities(bclinton) must not(containCapability(Capability.IsSudoer))
      }
    })

    taggedTest(it must "detect unused handles", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        registerUser(dao, "usedHandle")
        dao.profile.handleExists("usedHandle") must be (true)
        dao.profile.handleExists("unusedHandle") must be (false)
      }
    })

    taggedTest(it must "set machine quota for a given user", withDao { dao =>
      dao.store.withTransaction { implicit c =>
        val id1 = registerUser(dao, "jsmith").id
        val id2 = registerUser(dao, "bclinton").id
        val id3 = registerUser(dao, "imontoya").id
        val id4 = registerUser(dao, "vizzini").id

        Seq(id1, id2, id3, id4).foreach { id =>
          dao.profile.lookupByProfileId(id).get.quota must be (CosmosProfile.DefaultQuota)
        }

        dao.profile.setMachineQuota(id2, FiniteQuota(7))
        dao.profile.setMachineQuota(id3, UnlimitedQuota)
        dao.profile.setMachineQuota(id4, EmptyQuota)

        dao.profile.lookupByProfileId(id1).get.quota must be (CosmosProfile.DefaultQuota)
        dao.profile.lookupByProfileId(id2).get.quota must be (FiniteQuota(7))
        dao.profile.lookupByProfileId(id3).get.quota must be (UnlimitedQuota)
        dao.profile.lookupByProfileId(id4).get.quota must be (EmptyQuota)
      }
    })

    taggedTest(it must "lookup a profile from api credentials", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        registerUser(dao, "user4")
        val profileByUserId = dao.profile.lookupByUserId(userIdFor("user4")).get
        val profileByApiCredentials =
          dao.profile.lookupByApiCredentials(profileByUserId.apiCredentials).get
        profileByUserId must be (profileByApiCredentials)
      }
    })

    taggedTest(it must "lookup existing profiles by handle", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val profile = registerUser(dao, "handle")
        dao.profile.lookupByHandle("handle") must be (Some(profile))
        dao.profile.lookupByHandle("unknown") must not be 'defined
      }
    })

    taggedTest(it must "assign cluster ownership and remember it", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val clusterId = ClusterId()
        val id1 = registerUser(dao, "user1").id
        val id2 = registerUser(dao, "user2").id
        dao.cluster.register(clusterId, id2)
        dao.cluster.ownedBy(id1).map(_.clusterId).toList must not contain clusterId
        dao.cluster.ownedBy(id2).map(_.clusterId).toList must contain (clusterId)
      }
    })

    taggedTest(it must "retrieve the owner of a cluster", withDao { implicit dao =>
      val clusterId = ClusterId()
      val profileId = dao.store.withTransaction { implicit c =>
        val profileId = registerUser(dao, "user1").id
        dao.cluster.register(clusterId, profileId)
        profileId
      }
      dao.store.withTransaction { implicit c =>
        dao.cluster.ownerOf(clusterId) must be (Some(profileId))
      }
    })

    taggedTest(it must "get empty, NoGroup by default", withDao{ dao =>
      dao.store.withConnection{ implicit c =>
        dao.group.list() must equal (Set(NoGroup))
      }
    })

    taggedTest(it must "get all registered groups + NoGroup", withDao { dao =>
      val (a, b, c) = (
        GuaranteedGroup("A", EmptyQuota),
        GuaranteedGroup("B", Quota(2)),
        GuaranteedGroup("C", Quota(3))
      )
      dao.store.withTransaction{ implicit conn =>
        Seq(a, b, c).foreach(dao.group.register)
        dao.group.list() must be (Set(a, b, c, NoGroup))
      }
    })

    taggedTest(it must "set a user's group to an existing one", withDao { implicit dao =>
      val group = GuaranteedGroup("A", Quota(7))
      dao.store.withTransaction{ implicit c =>
        dao.group.register(group)
        val id1 = registerUser("user1").id
        registerUser("user2")
        dao.profile.lookupByUserId(userIdFor("user1")).get.group must be (NoGroup)
        dao.profile.lookupByUserId(userIdFor("user2")).get.group must be (NoGroup)
        dao.profile.setGroup(id1, Some(group.name))
        dao.profile.lookupByUserId(userIdFor("user1")).get.group must be (group)
        dao.profile.lookupByUserId(userIdFor("user2")).get.group must be (NoGroup)
      }
    })

    taggedTest(it must "delete an existing group", withDao { implicit dao =>
      val group: Group = GuaranteedGroup("A", Quota(7))
      dao.store.withTransaction{ implicit c =>
        dao.group.register(group)
        val id = registerUser("user1").id
        dao.profile.setGroup(id, Some(group.name))
        dao.group.delete(group.name)
        dao.group.list() must not (contain (group))
        dao.profile.lookupByUserId(userIdFor("user1")).get.group must be (NoGroup)
      }
    })

    taggedTest(it must "set the minimum quota of an existing group", withDao { implicit dao =>
      val group = GuaranteedGroup("A", Quota(7))
      dao.store.withTransaction { implicit c =>
        dao.group.register(group)
        val id = registerUser("user1").id
        dao.profile.setGroup(id, Some(group.name))
        dao.group.setQuota(group.name, EmptyQuota)
        dao.group.list().find(_.name == group.name).get.minimumQuota must be (EmptyQuota)
        dao.profile.lookupByUserId(userIdFor("user1")).get.group must
          be (GuaranteedGroup(group.name, EmptyQuota))
      }
    })

    taggedTest(it must "lookup users by group", withDao { implicit dao =>
      dao.store.withTransaction { implicit c =>
        val group = GuaranteedGroup("A", Quota(3))
        dao.group.register(group)
        val id1 = registerUser("user1").id
        val id2 = registerUser("user2").id
        dao.profile.setGroup(id2, Some(group.name))
        val noGroupProfiles = dao.profile.lookupByGroup(NoGroup)
        val groupProfiles = dao.profile.lookupByGroup(group)
        noGroupProfiles must have size 1
        noGroupProfiles.head.id must equal(id1)
        groupProfiles must have size 1
        groupProfiles.head.id must equal(id2)
      }
    })
  }
}
