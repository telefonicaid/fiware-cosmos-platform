/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.platform.ial.libvirt

import java.util.UUID
import scala.concurrent.Future

import es.tid.cosmos.platform.ial.MachineProfile

class FakeLibVirtServer(
    val properties: LibVirtServerProperties,
    domainCreated: Boolean = false) extends LibVirtServer {

  private var dom: Option[DomainProperties] = if (domainCreated) Some(domainProps()) else None

  private def domainProps() = {
    val uuid = UUID.randomUUID()
    DomainProperties(
      uuid = uuid,
      name = s"fake_${uuid.toString}",
      isActive = true,
      profile = MachineProfile.G1Compute,
      hostname = properties.domainHostname,
      ipAddress = properties.domainIpAddress
    )
  }

  def createDomain(): Future[DomainProperties] =
    if (dom.isDefined)
      Future.failed(new IllegalStateException("server is already created"))
    else {
      dom = Some(domainProps())
      Future.successful(dom.get)
    }

  def domain(): Future[DomainProperties] = Future.successful(dom.get)

  def isCreated(): Future[Boolean] = Future.successful(dom.isDefined)

  def destroyDomain(): Future[Unit] = Future.successful((dom = None))
}
