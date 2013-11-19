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
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.profile._

class ProfileTest extends FlatSpec with MustMatchers with MockitoSugar {

  trait WithMockCosmosProfileDao {
    val dao = mock[MockCosmosProfileDao]
    val registration = Registration("jsmith", "pk00001", "jsmith@example.com")
    dao.withConnection { implicit c =>
      dao.registerUser(UserId("db-0003"), registration)
    }
  }

  "A profile" must "unset quota" in new WithMockCosmosProfileDao {
    dao.withConnection { implicit c =>
      new Profile(dao).unsetMachineQuota(0) must be (true)
    }
  }

  it must "set a valid quota" in new WithMockCosmosProfileDao {
    dao.withConnection { implicit c =>
      new Profile(dao).setMachineQuota(0, 15) must be (true)
    }
  }
}
