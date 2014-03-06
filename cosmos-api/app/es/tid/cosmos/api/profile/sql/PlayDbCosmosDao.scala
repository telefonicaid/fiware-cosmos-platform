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

package es.tid.cosmos.api.profile.sql

import java.sql.Connection

import play.api.Play.current
import play.api.db.DB

import es.tid.cosmos.api.profile.CosmosDao

class PlayDbCosmosDao extends CosmosDao {

  type Conn = Connection

  def withConnection[A](block: (Conn) => A): A = DB.withConnection[A](block)
  def withTransaction[A](block: (Conn) => A): A = DB.withTransaction[A](block)

  override val profile = PlayDbProfileDao
  override val capability = PlayDbCapabilityDao
  override val group = PlayDbGroupDao
  override val cluster = PlayDbClusterDao
}
