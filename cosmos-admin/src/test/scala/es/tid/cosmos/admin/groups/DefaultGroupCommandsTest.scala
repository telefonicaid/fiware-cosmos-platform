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

package es.tid.cosmos.admin.groups

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.ClusterSecret
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.quota.{NoGroup, GuaranteedGroup, Quota, EmptyQuota}
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers

class DefaultGroupCommandsTest extends FlatSpec with MustMatchers with OneInstancePerTest {

  trait WithGroups extends MockCosmosDataStoreComponent {
    val serviceManager = new MockedServiceManager(maxPoolSize = 6)
    val groupCommands = new DefaultGroupCommands(store, serviceManager)
  }

  "Group commands" must "support creating a new group" in new WithGroups {
    groupCommands.create(name = "mygroup", minQuota = 3) must be ('success)
    groupCommands.create(name = "my-noquota-group", minQuota = 0) must be ('success)
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
    }
    groupCommands.list().message must include("groupA")
    groupCommands.list().message must include("groupB")
  }

  it must "support deleting an existing group" in new WithGroups {
    store.withTransaction { implicit c =>
      store.group.register(GuaranteedGroup("groupA", Quota(3)))
    }
    groupCommands.list().message must include("groupA")
    groupCommands.delete("groupA")
    groupCommands.list().message must be ("No groups available")
  }

  it must "reject deleting groups with shared clusters" in new WithGroups {
    val clusterId = serviceManager.defineCluster().view.id
    val groupA = GuaranteedGroup("groupA", Quota(3))
    store.withTransaction { implicit c =>
      store.group.register(groupA)
      val ownerId = CosmosProfileTestHelpers.registerUser(store, "user").id
      store.profile.setGroup(ownerId, groupA)
      store.cluster.register(clusterId, ownerId, ClusterSecret.random(), shared = true)
    }
    val result = groupCommands.delete(groupA.name)
    result must not be 'success
    result.message must include (
      s"Cannot delete group groupA: there are running shared clusters ($clusterId)")
  }

  it must "support setting an existing group's minimum quota" in new WithGroups {
    store.withTransaction { implicit c =>
      store.group.register(GuaranteedGroup("groupA", Quota(3)))
    }
    groupCommands.setMinQuota("groupA", 6)
    store.withTransaction { implicit c =>
      store.group.list() must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(6))))
    }
  }

  it must "validate feasibility before creating a new group" in new WithGroups {
    groupCommands.create("hugeGroup", 100) must not be 'success
    store.withTransaction { implicit c =>
      store.group.list() must be (Set(NoGroup))
    }
    groupCommands.create("validGroup", 6) must be ('success)
    store.withTransaction { implicit c =>
      store.group.list() must be (Set(NoGroup, GuaranteedGroup("validGroup", Quota(6))))
    }
  }

  it must "validate feasibility before setting a group's new minimum quota" in new WithGroups {
    val group = GuaranteedGroup("groupA", Quota(3))
    val (clusterId, _) = serviceManager.createCluster(ClusterName("myCluster"), 2, Set.empty, Seq.empty)
    store.withTransaction { implicit c =>
      val profile = registerUser(store, "myUser")
      store.group.register(group)
      store.cluster.register(clusterId, profile.id, ClusterSecret.random())
    }
    groupCommands.setMinQuota("groupA", 5) must not be 'success
    store.withTransaction { implicit c =>
      store.group.list() must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(3))))
    }
  }
}
