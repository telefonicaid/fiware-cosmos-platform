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

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.quota.{NoGroup, GuaranteedGroup, Quota, EmptyQuota}

class GroupsTest extends FlatSpec with MustMatchers with OneInstancePerTest {

  val dao = new MockCosmosProfileDao
  val groups = new Groups(dao)

  "Group commands" must "support creating a new group" in {
    groups.create(name = "mygroup", minQuota = 3) must be (true)
    groups.create(name = "my-noquota-group", minQuota = 0) must be (true)
    dao.withConnection { implicit c =>
      dao.getGroups must be (Set(
        NoGroup,
        GuaranteedGroup("mygroup", Quota(3)),
        GuaranteedGroup("my-noquota-group", EmptyQuota)
      ))
    }
  }

  it must "support listing existing groups" in { dao.withTransaction { implicit c =>
      dao.registerGroup(GuaranteedGroup("groupA", Quota(3)))
      dao.registerGroup(GuaranteedGroup("groupB", Quota(4)))
      groups.list must include("groupA")
      groups.list must include("groupB")
  }}

  it must "support deleting an existing group" in { dao.withTransaction { implicit c =>
    dao.registerGroup(GuaranteedGroup("groupA", Quota(3)))
    groups.list must include("groupA")
    groups.delete("groupA")
    groups.list must be ("No groups available")
  }}

  it must "support setting an existing group's minimum quota" in {
    dao.withTransaction { implicit c =>
      dao.registerGroup(GuaranteedGroup("groupA", Quota(3)))
      groups.setMinQuota("groupA", 6)
      dao.getGroups must be (Set(NoGroup, GuaranteedGroup("groupA", Quota(6))))
    }
  }
}
