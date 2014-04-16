/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.profile.dao

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.quota._

trait CosmosDataStoreBehavior
  extends CapabilityMatchers with MustMatchers with ClusterDataStoreBehavior { this: FlatSpec =>

  type ProfileTest = CosmosDataStore => Unit

  def profileDataStore(withStore: ProfileTest => Unit) {

    val unknownCosmosId = 1000

    it must "register new users" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val profile = registerUser(store, "jsmith")
        profile.handle must be ("jsmith")
        profile.keys.length must be (1)
        profile.email must be ("jsmith@example.com")
        profile.keys must be (Seq(NamedKey("default", s"ssh-rsa AAAAAA jsmith@example.com")))
        profile.quota must be (CosmosProfile.DefaultQuota)
        profile.group must be (CosmosProfile.DefaultGroup)
        profile.capabilities must be (CosmosProfile.DefaultUserCapabilities)
      }
    }

    it must "list all users" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        registerUser(store, "user1")
        registerUser(store, "user2")
        store.profile.list().map(_.handle) must be (Seq("user1", "user2"))
      }
    }

    it must "change the handle of users" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val cosmosId = registerUser(store, "jsmith").id
        store.profile.setHandle(cosmosId, "jsm")
        store.profile.lookupByUserId(userIdFor("jsmith")).get.handle must equal ("jsm")
      }
    }

    it must "not change the handle of unknown users" in withStore { store =>
      store.withTransaction { implicit c =>
        evaluating {
          store.profile.setHandle(unknownCosmosId, "jsm")
        } must produce [CosmosDaoException]
      }
    }

    it must "change the email of users" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val cosmosId = registerUser(store, "jsmith").id
        store.profile.setEmail(cosmosId, "new@mail.com")
        store.profile.lookupByUserId(userIdFor("jsmith")).get.email must equal ("new@mail.com")
      }
    }

    it must "not change the email of unknown users" in withStore { store =>
      store.withTransaction { implicit c =>
        evaluating {
          store.profile.setEmail(unknownCosmosId, "new@mail.com")
        } must produce [CosmosDaoException]
      }
    }

    it must "change the state of users" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val cosmosId = registerUser(store, "jsmith").id
        store.profile.setUserState(cosmosId, Deleting)
        store.profile.lookupByUserId(userIdFor("jsmith")).get.state must equal (Deleting)
      }
    }

    it must "not change the state of unknown users" in withStore { store =>
      store.withTransaction { implicit c =>
        evaluating {
          store.profile.setUserState(unknownCosmosId, Disabled)
        } must produce [CosmosDaoException]
      }
    }

    it must "not change the handle to a repeated one" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        registerUser("existing")
        val cosmosId = registerUser(store, "current").id
        evaluating {
          store.profile.setHandle(cosmosId, "existing")
        } must produce [CosmosDaoException]
      }
    }

    it must "change the keys of users" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val cosmosId = registerUser(store, "jsmith").id
        val newKeys = Seq(
          NamedKey("default", "ssh-rsa AAAAA jsmith@host"),
          NamedKey("extra", "ssh-rsa BBBBB jsmith@host"))
        store.profile.setPublicKeys(cosmosId, newKeys)
        store.profile.lookupByUserId(userIdFor("jsmith")).get.keys must equal (newKeys)
      }
    }

    it must "not change the keys of unknown users" in withStore { store =>
      store.withTransaction { implicit c =>
        evaluating {
          store.profile.setPublicKeys(
            unknownCosmosId, Seq(NamedKey("default", "ssh-rsa AAAAA jsmith@host")))
        } must produce [CosmosDaoException]
      }
    }

    it must "set user default capabilities to unstrusted" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val userProfile = registerUser(store, "jsmith")

        userProfile.capabilities must be (UntrustedUserCapabilities)
      }
    }

    it must "enable user capabilities" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val jsmith = registerUser(store, "jsmith").id
        val bclinton = registerUser(store, "bclinton").id

        store.capability.enable(bclinton, Capability.IsSudoer)

        store.capability.userCapabilities(jsmith) must not(containCapability(Capability.IsSudoer))
        store.capability.userCapabilities(bclinton) must containCapability(Capability.IsSudoer)
      }
    }

    it must "disable user capabilities" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val jsmith = registerUser(store, "jsmith").id
        val bclinton = registerUser(store, "bclinton").id

        store.capability.enable(bclinton, Capability.IsSudoer)
        store.capability.disable(bclinton, Capability.IsSudoer)

        store.capability.userCapabilities(jsmith) must not(containCapability(Capability.IsSudoer))
        store.capability.userCapabilities(bclinton) must not(containCapability(Capability.IsSudoer))
      }
    }

    it must "detect unused handles" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        registerUser(store, "usedHandle")
        store.profile.handleExists("usedHandle") must be (true)
        store.profile.handleExists("unusedHandle") must be (false)
      }
    }

    it must "set machine quota for a given user" in withStore { store =>
      store.withTransaction { implicit c =>
        val id1 = registerUser(store, "jsmith").id
        val id2 = registerUser(store, "bclinton").id
        val id3 = registerUser(store, "imontoya").id
        val id4 = registerUser(store, "vizzini").id

        Seq(id1, id2, id3, id4).foreach { id =>
          store.profile.lookupByProfileId(id).get.quota must be (CosmosProfile.DefaultQuota)
        }

        store.profile.setMachineQuota(id2, FiniteQuota(7))
        store.profile.setMachineQuota(id3, UnlimitedQuota)
        store.profile.setMachineQuota(id4, EmptyQuota)

        store.profile.lookupByProfileId(id1).get.quota must be (CosmosProfile.DefaultQuota)
        store.profile.lookupByProfileId(id2).get.quota must be (FiniteQuota(7))
        store.profile.lookupByProfileId(id3).get.quota must be (UnlimitedQuota)
        store.profile.lookupByProfileId(id4).get.quota must be (EmptyQuota)
      }
    }

    it must "lookup a profile from api credentials" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        registerUser(store, "user4")
        val profileByUserId = store.profile.lookupByUserId(userIdFor("user4")).get
        val profileByApiCredentials =
          store.profile.lookupByApiCredentials(profileByUserId.apiCredentials).get
        profileByUserId must be (profileByApiCredentials)
      }
    }

    it must "lookup existing profiles by handle" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val profile = registerUser(store, "handle")
        store.profile.lookupByHandle("handle") must be (Some(profile))
        store.profile.lookupByHandle("unknown") must not be 'defined
      }
    }

    it must "get empty, NoGroup by default" in withStore{ store =>
      store.withConnection{ implicit c =>
        store.group.list() must equal (Set(NoGroup))
      }
    }

    it must "get all registered groups + NoGroup" in withStore { store =>
      val (a, b, c) = (
        GuaranteedGroup("A", EmptyQuota),
        GuaranteedGroup("B", Quota(2)),
        GuaranteedGroup("C", Quota(3))
      )
      store.withTransaction { implicit conn =>
        Seq(a, b, c).foreach(store.group.register)
        store.group.list() must be (Set(a, b, c, NoGroup))
      }
    }

    it must "lookup groups by name" in withStore { store =>
      val group = GuaranteedGroup("name", Quota(10))
      store.withTransaction { implicit c =>
        store.group.register(group)
        store.group.lookupByName(group.name) must be (Some(group))
      }
    }

    it must "set a user's group to an existing one" in withStore { implicit store =>
      val group = GuaranteedGroup("A", Quota(7))
      store.withTransaction{ implicit c =>
        store.group.register(group)
        val id1 = registerUser("user1").id
        registerUser("user2")
        store.profile.lookupByUserId(userIdFor("user1")).get.group must be (NoGroup)
        store.profile.lookupByUserId(userIdFor("user2")).get.group must be (NoGroup)
        store.profile.setGroup(id1, Some(group.name))
        store.profile.lookupByUserId(userIdFor("user1")).get.group must be (group)
        store.profile.lookupByUserId(userIdFor("user2")).get.group must be (NoGroup)
      }
    }

    it must "delete an existing group" in withStore { implicit store =>
      val group: Group = GuaranteedGroup("A", Quota(7))
      store.withTransaction{ implicit c =>
        store.group.register(group)
        val id = registerUser("user1").id
        store.profile.setGroup(id, Some(group.name))
        store.group.delete(group.name)
        store.group.list() must not (contain (group))
        store.profile.lookupByUserId(userIdFor("user1")).get.group must be (NoGroup)
      }
    }

    it must "set the minimum quota of an existing group" in withStore { implicit store =>
      val group = GuaranteedGroup("A", Quota(7))
      store.withTransaction { implicit c =>
        store.group.register(group)
        val id = registerUser("user1").id
        store.profile.setGroup(id, Some(group.name))
        store.group.setQuota(group.name, EmptyQuota)
        store.group.list().find(_.name == group.name).get.minimumQuota must be (EmptyQuota)
        store.profile.lookupByUserId(userIdFor("user1")).get.group must
          be (GuaranteedGroup(group.name, EmptyQuota))
      }
    }

    it must "lookup users by group" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val group = GuaranteedGroup("A", Quota(3))
        store.group.register(group)
        val id1 = registerUser("user1").id
        val id2 = registerUser("user2").id
        store.profile.setGroup(id2, Some(group.name))
        val noGroupProfiles = store.profile.lookupByGroup(NoGroup)
        val groupProfiles = store.profile.lookupByGroup(group)
        noGroupProfiles must have size 1
        noGroupProfiles.head.id must equal(id1)
        groupProfiles must have size 1
        groupProfiles.head.id must equal(id2)
      }
    }
  }

}
