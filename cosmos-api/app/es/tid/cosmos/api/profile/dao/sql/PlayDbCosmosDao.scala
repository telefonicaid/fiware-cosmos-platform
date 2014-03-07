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

import java.sql.Connection

import es.tid.cosmos.api.profile.dao.CosmosDao

class PlayDbCosmosDao extends CosmosDao {

  type Conn = Connection

  override val store = PlayDbDataStore
  override val profile = PlayDbProfileDao
  override val capability = PlayDbCapabilityDao
  override val group = PlayDbGroupDao
  override val cluster = PlayDbClusterDao
}
