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

package es.tid.cosmos.api.profile.dao.mock

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.dao.{CosmosDataStoreBehavior, CosmosDataStore}

class MockCosmosDataStoreTest extends FlatSpec with MustMatchers with CosmosDataStoreBehavior {

   def withMockCosmosProfileDao(block: CosmosDataStore => Unit) {
     block(new MockCosmosDataStore())
   }

   "The mocked profile resource" must behave like profileDataStore(withMockCosmosProfileDao)
 }
