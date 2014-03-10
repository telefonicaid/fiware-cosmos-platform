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

import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent

/** Use Play DB as data store for Cosmos profile, groups, etc. */
trait PlayDbDataStoreComponent extends CosmosDataStoreComponent {
  override lazy val store = PlayDbCosmosDataStore
}
