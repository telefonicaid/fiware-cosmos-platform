package es.tid.cosmos.platform.ial.libvirt

import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.platform.common.MySqlDatabase
import es.tid.cosmos.platform.ial.serverpool.{ServerPoolInfrastructureProvider, SqlServerPoolDao}
import es.tid.cosmos.platform.ial.libvirt.jna.JnaLibVirtServer


/**
 * A trait satisfying infrastructure provider component supported by libvirt.
 */
trait LibVirtInfrastructureProviderComponent extends InfrastructureProviderComponent {

  override lazy val infrastructureProvider = {
    val dbHost = "localhost"
    val dbPort = 3306
    val dbUser = "cosmos"
    val dbPassword = "cosmos"
    val dbName = "cosmos"
    val db = new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, dbName)
    val dao = new SqlLibVirtDao(db)
    new LibVirtInfrastructureProvider(dao, props => new JnaLibVirtServer(props))
  }
}
