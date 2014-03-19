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

package es.tid.cosmos.admin

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.quota._

class ProfileCommandsTest extends FlatSpec with MustMatchers {

  trait WithMockCosmosProfileDao extends MockCosmosDataStoreComponent {
    val handle = "jsmith"
    val cosmosId = registerUser(handle)(store).id
    val instance = new ProfileCommands(store)
    def userProfile = store.withTransaction { implicit c =>
      store.profile.lookupByHandle(handle)
    }.get
  }

  "A profile" must "unset quota" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.profile.setMachineQuota(cosmosId, EmptyQuota)
    }
    instance.removeMachineQuota(handle) must be (true)
    userProfile.quota must be (UnlimitedQuota)
  }

  it must "set a valid quota" in new WithMockCosmosProfileDao {
    instance.setMachineQuota(handle, 15) must be (true)
    userProfile.quota must be (Quota(15))
  }

  it must "enable user capabilities" in new WithMockCosmosProfileDao {
    instance.enableCapability(handle, "is_sudoer") must be (true)
    userProfile.capabilities.hasCapability(Capability.IsSudoer) must be (true)
  }

  it must "disable user capabilities" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.capability.enable(cosmosId, Capability.IsSudoer)
    }
    instance.disableCapability(handle, "is_sudoer") must be (true)
    userProfile.capabilities.hasCapability(Capability.IsSudoer) must be (false)
  }

  it must "set to an existing group" in new WithMockCosmosProfileDao {
    val group = GuaranteedGroup("mygroup", Quota(3))
    store.withTransaction{ implicit c =>
      store.group.register(group)
      new ProfileCommands(store).setGroup(handle, group.name) must be (true)
      userProfile.group must be (group)
    }
  }

  it must "remove user from its assigned group" in new WithMockCosmosProfileDao {
    val group = GuaranteedGroup("mygroup", Quota(3))
    store.withTransaction{ implicit c =>
      store.group.register(group)
      store.profile.setGroup(cosmosId, Some("mygroup"))
      new ProfileCommands(store).removeGroup(handle) must be (true)
      userProfile.group must be (NoGroup)
    }
  }

  it must "list existing profile handles ordered alphabetically" in new WithMockCosmosProfileDao {
    registerUser("imontoya")(store)
    store.withTransaction { implicit c =>
      new ProfileCommands(store).list must include("imontoya\njsmith")
    }
  }

  it must "list no handles when there are no users" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.profile.list().foreach { profile =>
        store.profile.setUserState(profile.id, UserState.Deleted)
      }
      new ProfileCommands(store).list must equal("No users found")
    }
  }
}
