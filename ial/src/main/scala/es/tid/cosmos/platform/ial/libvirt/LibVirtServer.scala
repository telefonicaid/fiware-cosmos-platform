package es.tid.cosmos.platform.ial.libvirt

import scala.concurrent.Future

/**
 * An abstraction for the capabilities of a libvirt server.
 */
trait LibVirtServer {
  /**
   * Create a new libvirt domain.
   *
   * @return the future properties of the newly created domain
   */
  def createDomain(): Future[DomainProperties]

  /**
   * Indicates whether the domain is created
   */
  def isCreated(): Future[Boolean]

  /**
   * Destroy the existing domain.
   *
   * @return the future representing the destruction process
   */
  def destroyDomain(): Future[Unit]
}
