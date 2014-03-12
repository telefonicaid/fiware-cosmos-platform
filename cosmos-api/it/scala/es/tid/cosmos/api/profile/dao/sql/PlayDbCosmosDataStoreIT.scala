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

package es.tid.cosmos.api.profile.dao.sql

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.specs2.execute.{Result, AsResult}
import play.api.test.Helpers

import es.tid.cosmos.api.profile.WithTestDatabase
import es.tid.cosmos.api.profile.dao.{CosmosDataStoreBehavior, CosmosDataStore}
import es.tid.cosmos.common.scalatest.tags.{TaggedTests, HasExternalDependencies}

class PlayDbCosmosDataStoreIT
  extends FlatSpec with MustMatchers with CosmosDataStoreBehavior with TaggedTests {

  override val testsTag = HasExternalDependencies

  def withStore(block: (CosmosDataStore) => Unit): Unit = new WithTestDatabase() {

    override def around[T: AsResult](t: => T): Result = Helpers.running(app) {
      // This code block should work without overriding the around method but the compiler
      // breaks down due to the interaction of closure capturing and the DelayedInit inherited
      // from [[play.api.test.WithTestApplication]].
      resetDb()
      block(PlayDbCosmosDataStore)

      AsResult.effectively(t)
    }
  }

  "The Play DB profile resource" must behave like profileDataStore(withStore)

  it must behave like clusterDataStore(withStore)
}
