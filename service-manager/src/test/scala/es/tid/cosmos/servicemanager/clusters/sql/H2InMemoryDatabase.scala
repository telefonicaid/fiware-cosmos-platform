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
package es.tid.cosmos.servicemanager.clusters.sql

import java.sql.{Connection, DriverManager}
import scala.util.Try

import org.squeryl.adapters.H2Adapter

import es.tid.cosmos.common.SqlDatabase

class H2InMemoryDatabase extends SqlDatabase {

  DriverManager.registerDriver(new org.h2.Driver)

  def connect: Try[Connection] = Try(DriverManager.getConnection(
    "jdbc:h2:mem:ClustersTest;MODE=MYSQL", "", ""))

  override val adapter = new H2Adapter

}
