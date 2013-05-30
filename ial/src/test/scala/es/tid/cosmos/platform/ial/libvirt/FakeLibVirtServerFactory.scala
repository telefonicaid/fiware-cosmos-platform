package es.tid.cosmos.platform.ial.libvirt

/**
 * A libvirt server factory which returns the same server for each server name.
 */
class FakeLibVirtServerFactory {

  private val servers = scala.collection.mutable.Map[String, LibVirtServer]()

  def apply(properties: LibVirtServerProperties) : LibVirtServer = {
    servers.get(properties.name) match {
      case Some(server) => server
      case None => {
        val srv = new FakeLibVirtServer(properties)
        servers.put(properties.name, srv)
        srv
      }
    }
  }
}
