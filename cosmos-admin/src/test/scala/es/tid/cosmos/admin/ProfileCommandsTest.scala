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

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManagerComponent
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.quota._
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.Running

class ProfileCommandsTest extends FlatSpec with MustMatchers with CapabilityMatchers {

  trait WithMockCosmosProfileDao extends MockCosmosDataStoreComponent
      with MockedServiceManagerComponent {
    val handle = "jsmith"
    val cosmosId = registerUser(handle)(store).id
    val instance = new ProfileCommands(store, serviceManager)
    val group = GuaranteedGroup("mygroup", Quota(3))

    def userProfile() = store.withTransaction { implicit c =>
      store.profile.lookupByHandle(handle)
    }.get
  }

  "A profile" must "unset quota" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.profile.setMachineQuota(cosmosId, EmptyQuota)
    }
    instance.removeMachineQuota(handle) must be ('success)
    userProfile().quota must be (UnlimitedQuota)
  }

  it must "set a valid quota" in new WithMockCosmosProfileDao {
    instance.setMachineQuota(handle, 15) must be ('success)
    userProfile().quota must be (Quota(15))
  }

  it must "enable user capabilities" in new WithMockCosmosProfileDao {
    instance.enableCapability(handle, "is_sudoer") must be ('success)
    userProfile().capabilities must containCapability(Capability.IsSudoer)
  }

  it must "disable user capabilities" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.capability.enable(cosmosId, Capability.IsSudoer)
    }
    instance.disableCapability(handle, "is_sudoer") must be ('success)
    userProfile().capabilities.hasCapability(Capability.IsSudoer) must not  be 'success
  }

  it must "set to an existing group" in new WithMockCosmosProfileDao {
    store.withTransaction{ implicit c =>
      store.group.register(group)
      instance.setGroup(handle, group.name) must be ('success)
      userProfile().group must be (group)
    }
  }

  it must "remove user from its assigned group" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.group.register(group)
      store.profile.setGroup(cosmosId, group)
    }
    instance.removeGroup(handle) must be ('success)
    store.withTransaction { implicit c =>
      userProfile().group must be (NoGroup)
    }
  }

  it must "reject removing user from group if he owns a shared cluster" in
    new WithMockCosmosProfileDao {
      val clusterId = mockedServiceManager.defineCluster().view.id
      store.withTransaction { implicit c =>
        store.group.register(group)
        store.profile.setGroup(cosmosId, group)
        store.cluster.register(clusterId, cosmosId, shared = true)
      }
      val result = instance.removeGroup(handle)
      result must not be 'success
      result.message must include (clusterId.toString)
    }

  it must "disable user from shared clusters when leaving a group" in
    new WithMockCosmosProfileDao {
      val cluster = store.withTransaction { implicit c =>
        store.group.register(group)
        store.profile.setGroup(cosmosId, group)
        val ownerId = registerUser(store, "owner").id
        store.profile.setGroup(ownerId, group)
        val fakeCluster = mockedServiceManager.defineCluster(
          users = Set(ClusterUser("owner", "key1"), ClusterUser(handle, "key2")),
          initialState = Some(Running)
        )
        store.cluster.register(fakeCluster.view.id, ownerId, shared = true)
        fakeCluster
      }
      instance.removeGroup(handle) must be ('success)
      store.withTransaction { implicit c =>
        store.profile.lookupByProfileId(cosmosId)
      }.map(_.group) must be (Some(NoGroup))
      val disabledUser = cluster.view.users.get.find(_.username == handle)
      disabledUser.map(_.isEnabled) must be (Some(false))
    }

  it must "list existing profile handles ordered alphabetically" in new WithMockCosmosProfileDao {
    registerUser("imontoya")(store)
    instance.list().message must include("imontoya\njsmith")
  }

  it must "list no handles when there are no users" in new WithMockCosmosProfileDao {
    store.withTransaction { implicit c =>
      store.profile.list().foreach { profile =>
        store.profile.setUserState(profile.id, UserState.Deleted)
      }
    }
    instance.list().message must equal("No users found")
  }
}
