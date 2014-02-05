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

import es.tid.cosmos.common.scalatest.tags.HasExternalDependencies

class CosmosProfileDaoIT extends FlatSpec with MustMatchers with CosmosProfileDaoBehavior {

  def withPlayDbCosmosProfileDao(block: CosmosProfileDao => Unit) {
    val environment = new WithTestDatabase()
    Helpers.running(environment.appWithTestDb) {
      environment.resetDb()
      block(new PlayDbCosmosProfileDao)
    }
  }

  "The Play DB profile resource" must behave like
    profileDao(withPlayDbCosmosProfileDao, Some(HasExternalDependencies))

  def withMockCosmosProfileDao(block: CosmosProfileDao => Unit) {
    block(new MockCosmosProfileDao)
  }

  "The mocked profile resource" must behave like profileDao(withMockCosmosProfileDao)
}
