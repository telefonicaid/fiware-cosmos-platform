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
