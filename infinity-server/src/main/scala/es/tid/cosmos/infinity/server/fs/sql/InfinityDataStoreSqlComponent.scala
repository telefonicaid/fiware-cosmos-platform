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
