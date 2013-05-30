package es.tid.cosmos.platform.ial.libvirt

/**
 * A data access object which provides metadata of libvirt infrastructure.
 */
trait LibVirtDao {
  /**
   * Obtain the list of available libvirt servers
   */
  def libVirtServers : Seq[LibVirtServerProperties]
}
