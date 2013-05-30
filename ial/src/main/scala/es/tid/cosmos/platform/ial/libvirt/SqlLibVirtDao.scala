package es.tid.cosmos.platform.ial.libvirt

import org.squeryl._
import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.platform.common.SqlDatabase
import es.tid.cosmos.platform.ial.MachineProfile

protected class PersistentLibVirtServerProperties(
    val name: String,
    val description: String,
    val profile: MachineProfile.Value,
    val connectionChain: String,
    val numberOfCpus: Int,
    val totalMemory: Int,
    val domainTemplate: String,
    val bridgeName: String,
    val domainHostname: String,
    val domainIpAddress: String,
    val enabled: Boolean) extends LibVirtServerProperties with KeyedEntity[Long] {
  override val id: Long = 0

  /**
   * Fake constructor required by Squeryl to map the class and the table name.
   */
  def this() = this("", "", MachineProfile.M, "", 0, 0, "", "", "", "", true)

  def toTransient = new TransientLibVirtServerProperties(
    name, description, profile, connectionChain, numberOfCpus, totalMemory,
    domainTemplate, bridgeName, domainHostname, domainIpAddress)
}

object LibVirtDb extends Schema {
  val servers = table[PersistentLibVirtServerProperties]("SERVERS")
}

/**
 * A data access object for libvirt supported by an SQL database.
 */
class SqlLibVirtDao(val db: SqlDatabase) extends LibVirtDao {
  import LibVirtDb._

  /**
   * Obtain the list of available libvirt servers
   */
  def libVirtServers: Seq[LibVirtServerProperties] = transaction(db.newSession) {
    from(servers)(srv => where(srv.enabled === true) select(srv))
      .map(_.toTransient).toSeq
  }
}
