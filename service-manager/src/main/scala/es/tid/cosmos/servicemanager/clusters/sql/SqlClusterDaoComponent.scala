package es.tid.cosmos.servicemanager.clusters.sql

import com.imageworks.migration.{InstallAllMigrations, Migrator, MysqlDatabaseAdapter}

import es.tid.cosmos.common.{ConfigComponent, MySqlDatabase, MySqlConnDetails}
import es.tid.cosmos.servicemanager.clusters.{ClusterDao, ClusterDaoComponent}

trait SqlClusterDaoComponent extends ClusterDaoComponent { this: ConfigComponent =>
  private val connDetails = MySqlConnDetails.fromConfig(config)
  private val db = new MySqlDatabase(connDetails)
  private val migrator = new Migrator(
    connDetails.asJdbc,
    connDetails.username,
    connDetails.password,
    new MysqlDatabaseAdapter(schemaNameOpt = None))
  migrator.migrate(
    InstallAllMigrations,
    packageName = "es.tid.cosmos.servicemanager.clusters.sql.migrations",
    searchSubPackages = false)
  override val serviceManagerClusterDao: ClusterDao = new SqlClusterDao(db)
}
