/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.platform.ial.libvirt

import scala.concurrent.Future

trait LibVirtServer {
  /**
   * Create a new libvirt domain.
   *
   * @return the future properties of the newly created domain
   */
  def createDomain(): Future[DomainProperties]

  /**
   * Get the existing domain for this server. If there is no such domain, a NoSuchElementException
   * is encapsulated as error in the resulting future.
   *
   * @return the properties of the existing domain, or NoSuchElementException error if there is
   *         no domain for this server
   */
  def domain(): Future[DomainProperties]

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
