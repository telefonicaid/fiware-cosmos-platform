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

package es.tid.cosmos.admin.groups

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.quota.{NoGroup, GuaranteedGroup, Quota, EmptyQuota}
import es.tid.cosmos.servicemanager.ClusterName

class GroupOperationsTest extends FlatSpec with MustMatchers with OneInstancePerTest {

  trait WithGroups extends MockCosmosDataStoreComponent {
    val serviceManager = new MockedServiceManager(maxPoolSize = 6)
    val groups = new GroupOperations(store, serviceManager)
  }

  "Group commands" must "support creating a new group" in new WithGroups {
    groups.create(name = "mygroup", minQuota = 3) must be (true)
    groups.create(name = "my-noquota-group", minQuota = 0) must be (true)
    store.withConnection { implicit c =>
      store.group.list() must be (Set(
        NoGroup,
        GuaranteedGroup("mygroup", Quota(3)),
        GuaranteedGroup("my-noquota-group", EmptyQuota)
      ))
    }
  }

  it must "support listing existing groups" in new WithGroups {
    store.withTransaction { implicit c =>
      store.group.register(GuaranteedGroup("groupA", Quota(3)))
      store.group.register(GuaranteedGroup("groupB", Quota(4)))
      groups.list must include("groupA")
      groups.list must include("groupB")
    }
  }

  it must "support deleting an existing group" in new WithGroups {
    store.withTransaction { implicit c =>
      store.group.register(GuaranteedGroup("groupA", Quota(3)))
      groups.list must include("groupA")
      groups.delete("groupA")
      groups.list must be ("No groups available")
    }
  }

  it must "support setting an existing group's minimum quota" in new WithGroups {
    store.withTransaction { implicit c =>
      store.group.register(GuaranteedGroup("groupA", Quota(3)))
      groups.setMinQuota("groupA", 6)
      store.group.list() must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(6))))
    }
  }

  it must "validate feasibility before creating a new group" in new WithGroups {
    store.withTransaction { implicit c =>
      groups.create("hugeGroup", 100) must be (false)
      store.group.list() must be (Set(NoGroup))
      groups.create("validGroup", 6) must be (true)
      store.group.list() must be (Set(NoGroup, GuaranteedGroup("validGroup", Quota(6))))
    }
  }

  it must "validate feasibility before setting a group's new minimum quota" in new WithGroups {
    val group = GuaranteedGroup("groupA", Quota(3))
    val clusterId = serviceManager.createCluster(ClusterName("myCluster"), 2, Set.empty, Seq.empty)
    store.withTransaction { implicit c =>
      val profile = registerUser(store, "myUser")
      store.group.register(group)
      store.cluster.register(clusterId, profile.id)
      groups.setMinQuota("groupA", 5) must be (false)
      store.group.list() must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(3))))
    }
  }
}
