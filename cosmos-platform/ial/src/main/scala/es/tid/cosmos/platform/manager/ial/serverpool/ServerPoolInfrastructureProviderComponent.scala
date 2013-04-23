package es.tid.cosmos.platform.manager.ial.serverpool

import scala.util.Random

import es.tid.cosmos.platform.manager.ial.{InfrastructureProvider, InfrastructureProviderComponent}

/**
 * @author sortega
 */
trait ServerPoolInfrastructureProviderComponent extends InfrastructureProviderComponent {

  override lazy val infrastructureProvider = {
    val dbHost = "localhost"
    val dbPort = 3306
    val dbUser = "cosmos"
    val dbPassword = "cosmos"
    val dbName = "cosmos"
    val db = new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, dbName)
    val dao = new SqlServerPoolDao(db)
    new ServerPoolInfrastructureProvider(dao)
  }
}
