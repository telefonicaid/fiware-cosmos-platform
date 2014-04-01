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
package es.tid.cosmos.infinity.server.fs.sql

import es.tid.cosmos.infinity.server.db.sql.DataStoreSql
import es.tid.cosmos.infinity.server.fs.InodeDataStore

trait InodeDataStoreSql extends InodeDataStore { self: DataStoreSql =>
  override val inodeDao = new InodeDaoSql
}
