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

import java.util.UUID
import scala.concurrent.Future

import es.tid.cosmos.platform.ial.MachineProfile

/**
 * @author apv
 */
class FakeLibVirtServer(val properties: LibVirtServerProperties) extends LibVirtServer {

  private var dom: Option[DomainProperties] = None

  def createDomain(): Future[DomainProperties] =
    if (dom.isDefined)
      Future.failed(new IllegalStateException("server is already created"))
    else {
      val uuid = UUID.randomUUID()

      dom = Some(DomainProperties(
        uuid = uuid,
        name = s"fake_${uuid.toString}",
        isActive = true,
        profile = MachineProfile.G1Compute,
        hostname = properties.domainHostname,
        ipAddress = properties.domainIpAddress
      ))

      Future.successful(dom.get)
    }

  def domain(): Future[DomainProperties] = Future.successful(dom.get)

  def isCreated(): Future[Boolean] = Future.successful(dom.isDefined)

  def destroyDomain(): Future[Unit] = Future.successful((dom = None))
}
