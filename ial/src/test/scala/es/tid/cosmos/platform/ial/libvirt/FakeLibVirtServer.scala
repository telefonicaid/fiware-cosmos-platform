package es.tid.cosmos.platform.ial.libvirt

import scala.concurrent.Future
import java.util.UUID

/**
 * @author apv
 */
class FakeLibVirtServer(val properties: LibVirtServerProperties) extends LibVirtServer {

  var created: Boolean = false

  def createDomain(): Future[DomainProperties] = {
    if (created)
      Future.failed(new IllegalStateException("server is already created"))
    else {
      val uuid = UUID.randomUUID()
      created = true
      Future.successful(DomainProperties(
        uuid = uuid,
        name = s"fake_${uuid.toString}",
        isActive = true,
        hostname = properties.domainHostname,
        ipAddress = properties.domainIpAddress
      ))
    }
  }

  def isCreated(): Future[Boolean] = Future.successful(created)

  def destroyDomain(): Future[Unit] = Future.successful((created = false))
}
