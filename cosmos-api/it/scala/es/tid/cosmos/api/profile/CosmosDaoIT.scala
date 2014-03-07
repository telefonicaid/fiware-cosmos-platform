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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers

import es.tid.cosmos.api.profile.sql.PlayDbCosmosDao
import es.tid.cosmos.common.scalatest.tags.HasExternalDependencies

class CosmosDaoIT extends FlatSpec with MustMatchers with CosmosDaoBehavior {

  def withPlayDbCosmosProfileDao(block: CosmosDao => Unit) {
    val environment = new WithTestDatabase()
    Helpers.running(environment.appWithTestDb) {
      environment.resetDb()
      block(new PlayDbCosmosDao)
    }
  }

  "The Play DB profile resource" must behave like
    profileDao(withPlayDbCosmosProfileDao, Some(HasExternalDependencies))

  def withMockCosmosProfileDao(block: CosmosDao => Unit) {
    block(new MockCosmosDao)
  }

  "The mocked profile resource" must behave like profileDao(withMockCosmosProfileDao)
}
