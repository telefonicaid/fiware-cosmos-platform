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
    instance.enableCapability(handle, Capability.IsSudoer) must be (true)
    userProfile.capabilities.hasCapability(Capability.IsSudoer) must be (true)
  }

  it must "disable user capabilities" in new WithMockCosmosProfileDao {
    dao.withTransaction { implicit c =>
      dao.enableUserCapability(cosmosId, Capability.IsSudoer)
    }
    instance.disableCapability(handle, Capability.IsSudoer) must be (true)
    userProfile.capabilities.hasCapability(Capability.IsSudoer) must be (false)
  }

  it must "set to an existing group" in new WithMockCosmosProfileDao {
    val group = GuaranteedGroup("mygroup", Quota(3))
    dao.withTransaction{ implicit c =>
      dao.registerGroup(group)
      new Profile(dao).setGroup(handle, Some(group.name)) must be (true)
      userProfile.group must be (group)
    }
  }
}
