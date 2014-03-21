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

package es.tid.cosmos.admin.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent

class ListProfilesCommandTest extends FlatSpec with MustMatchers {

  trait WithMockCosmosProfileDao extends MockCosmosDataStoreComponent {
    val listCommand = new ListProfilesCommand(store)
  }

  it must "list no handles when there are no users" in new WithMockCosmosProfileDao {
    listCommand().message must equal("No users found")
  }

  it must "list existing profile handles ordered alphabetically" in new WithMockCosmosProfileDao {
    registerUser("jsmith")(store)
    registerUser("imontoya")(store)
    listCommand().message must include("imontoya\njsmith")
  }
}
