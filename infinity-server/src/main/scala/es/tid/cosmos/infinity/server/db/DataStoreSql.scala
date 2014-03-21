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

package es.tid.cosmos.infinity.server.db

import java.sql.Connection
import javax.sql.DataSource

trait DataStoreSql extends DataStore {

  val datasource: DataSource

  override type Conn = Connection

  override def withConnection[A](block: Conn => A): A =
    DB.withConnection(datasource.getConnection)(block)

  override def withTransaction[A](block: Conn => A): A =
    DB.withTransaction(datasource.getConnection)(block)

}
