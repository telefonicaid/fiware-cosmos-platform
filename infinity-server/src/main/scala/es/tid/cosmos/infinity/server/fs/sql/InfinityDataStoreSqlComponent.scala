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

import com.imageworks.migration.{InstallAllMigrations, MysqlDatabaseAdapter, Migrator}
import scalikejdbc.ConnectionPool

import es.tid.cosmos.common.{MySqlConnDetails, ConfigComponent}
import es.tid.cosmos.infinity.server.db.InfinityDataStore
import es.tid.cosmos.infinity.server.db.sql.migrations.Migrate_1_InitialVersion
import es.tid.cosmos.infinity.server.fs.InfinityDataStoreComponent

trait InfinityDataStoreSqlComponent extends InfinityDataStoreComponent { this: ConfigComponent =>

  override lazy val infinityDataStore: InfinityDataStore = {
    val infinityConfig = config.getConfig("cosmos.infinity.server")
    val connectionDetails = MySqlConnDetails.fromConfig(infinityConfig)
    applyMigrations(connectionDetails)
    ConnectionPool.singleton(
      connectionDetails.asJdbc, connectionDetails.username, connectionDetails.password)
    new InfinityDataStoreSql(ConnectionPool.dataSource())
  }

  private def applyMigrations(details: MySqlConnDetails): Unit = {
    val migrator = new Migrator(
      details.asJdbc,
      details.username,
      details.password,
      new MysqlDatabaseAdapter(schemaNameOpt = None)
    )
    migrator.migrate(
      InstallAllMigrations,
      packageName = classOf[Migrate_1_InitialVersion].getPackage.getName,
      searchSubPackages = false
    )
  }
}
