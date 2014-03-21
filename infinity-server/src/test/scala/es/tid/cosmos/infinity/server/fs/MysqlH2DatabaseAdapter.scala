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

package es.tid.cosmos.infinity.server.fs

import com.imageworks.migration.MysqlDatabaseAdapter

/** Adapter for scala-migrations for H2 based on Mysql, as H2 lacks of LOCK statement. */
class MysqlH2DatabaseAdapter(override val schemaNameOpt: Option[String])
    extends MysqlDatabaseAdapter(schemaNameOpt) {

  override def lockTableSql(tableName: String): String =
    // see http://www.h2database.com/html/features.htm section "Locking, Lock-Timeout, Deadlocks"
    "SELECT * FROM SYSTEM_RANGE(1,2) WHERE 1=0 FOR UPDATE;"

}
