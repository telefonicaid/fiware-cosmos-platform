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

import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.mockito.Mockito.spy
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._

class ProfileTest extends FlatSpec with MustMatchers {

  trait WithMockCosmosProfileDao {
    val dao = new MockCosmosProfileDao()
    val handle = "jsmith"
    val cosmosId = registerUser(handle)(dao).id
    val instance = new Profile(dao)
    def userProfile = dao.withTransaction { implicit c =>
      dao.lookupByHandle(handle)
    }.get
  }

  "A profile" must "unset quota" in new WithMockCosmosProfileDao {
    dao.withTransaction { implicit c =>
      dao.setMachineQuota(cosmosId, EmptyQuota)
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
    dao.withTransaction { implicit c =>
      dao.enableUserCapability(cosmosId, Capability.IsSudoer)
    }
    instance.disableCapability(handle, "is_sudoer") must be (true)
    userProfile.capabilities.hasCapability(Capability.IsSudoer) must be (false)
  }

  it must "set to an existing group" in new WithMockCosmosProfileDao {
    val group = GuaranteedGroup("mygroup", Quota(3))
    dao.withTransaction{ implicit c =>
      dao.registerGroup(group)
      new Profile(dao).setGroup(handle, group.name) must be (true)
      userProfile.group must be (group)
    }
  }

  it must "remove user from its assigned group" in new WithMockCosmosProfileDao {
    val group = GuaranteedGroup("mygroup", Quota(3))
    dao.withTransaction{ implicit c =>
      dao.registerGroup(group)
      dao.setGroup(cosmosId, Some("mygroup"))
      new Profile(dao).removeGroup(handle) must be (true)
      userProfile.group must be (NoGroup)
    }
  }

  it must "list existing profile handles ordered alphabetically" in new WithMockCosmosProfileDao {
    registerUser("imontoya")(dao)
    dao.withTransaction { implicit c =>
      new Profile(dao).list must include("imontoya\njsmith")
    }
  }

  it must "list no handles when there are no users" in new WithMockCosmosProfileDao {
    val spicedDao = spy(dao)
    given(spicedDao.getAllUsers()(any())).willReturn(Nil)
    spicedDao.withTransaction { implicit c =>
      new Profile(spicedDao).list must equal("No users found")
    }
  }
}
