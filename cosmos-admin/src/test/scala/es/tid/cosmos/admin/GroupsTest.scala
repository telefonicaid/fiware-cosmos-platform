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

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDao
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.quota.{NoGroup, GuaranteedGroup, Quota, EmptyQuota}
import es.tid.cosmos.servicemanager.ClusterName

class GroupsTest extends FlatSpec with MustMatchers with OneInstancePerTest {

  val dao = new MockCosmosDao
  val serviceManager = new MockedServiceManager(maxPoolSize = 6)
  val groups = new Groups(dao, serviceManager)

  "Group commands" must "support creating a new group" in {
    groups.create(name = "mygroup", minQuota = 3) must be (true)
    groups.create(name = "my-noquota-group", minQuota = 0) must be (true)
    dao.store.withConnection { implicit c =>
      dao.group.list() must be (Set(
        NoGroup,
        GuaranteedGroup("mygroup", Quota(3)),
        GuaranteedGroup("my-noquota-group", EmptyQuota)
      ))
    }
  }

  it must "support listing existing groups" in { dao.store.withTransaction { implicit c =>
      dao.group.register(GuaranteedGroup("groupA", Quota(3)))
      dao.group.register(GuaranteedGroup("groupB", Quota(4)))
      groups.list must include("groupA")
      groups.list must include("groupB")
  }}

  it must "support deleting an existing group" in { dao.store.withTransaction { implicit c =>
    dao.group.register(GuaranteedGroup("groupA", Quota(3)))
    groups.list must include("groupA")
    groups.delete("groupA")
    groups.list must be ("No groups available")
  }}

  it must "support setting an existing group's minimum quota" in {
    dao.store.withTransaction { implicit c =>
      dao.group.register(GuaranteedGroup("groupA", Quota(3)))
      groups.setMinQuota("groupA", 6)
      dao.group.list() must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(6))))
    }
  }

  it must "validate feasibility before creating a new group" in {
    dao.store.withTransaction { implicit c =>
      groups.create("hugeGroup", 100) must be (false)
      dao.group.list() must be (Set(NoGroup))
      groups.create("validGroup", 6) must be (true)
      dao.group.list() must be (Set(NoGroup, GuaranteedGroup("validGroup", Quota(6))))
    }
  }

  it must "validate feasibility before setting a group's new minimum quota" in {
    val group = GuaranteedGroup("groupA", Quota(3))
    val clusterId = serviceManager.createCluster(ClusterName("myCluster"), 2, Seq.empty, Seq.empty)
    dao.store.withTransaction { implicit c =>
      val profile = registerUser(dao, "myUser")
      dao.group.register(group)
      dao.cluster.register(clusterId, profile.id)
      groups.setMinQuota("groupA", 5) must be (false)
      dao.group.list() must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(3))))
    }
  }
}
