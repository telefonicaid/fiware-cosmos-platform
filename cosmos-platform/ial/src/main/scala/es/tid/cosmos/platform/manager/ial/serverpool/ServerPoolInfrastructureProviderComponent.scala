package es.tid.cosmos.platform.manager.ial.serverpool

import scala.util.Random

import es.tid.cosmos.platform.manager.ial.{InfrastructureProvider, InfrastructureProviderComponent}

/**
 * @author sortega
 */
trait ServerPoolInfrastructureProviderComponent extends InfrastructureProviderComponent {
  val dbHost = "localhost"
  val dbPort = 3306
  val dbUser = "cosmos"
  val dbPassword = "cosmos"
  val dbName = s"test_${math.abs(Random.nextInt())}"
  private lazy val db = new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, dbName)
  private lazy val dao = new SqlServerPoolDao(db)
  lazy val infrastructureProvider: InfrastructureProvider =
    new ServerPoolInfrastructureProvider(dao)
}
