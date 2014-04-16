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

package es.tid.cosmos.platform.ial.libvirt.jna

import java.util.{NoSuchElementException, UUID}

import scala.concurrent.{blocking, Future, future}
import scala.concurrent.ExecutionContext.Implicits.global

import org.libvirt.{Connect, LibvirtException, Domain}
import org.libvirt.Error.ErrorNumber

import es.tid.cosmos.common.SequentialOperations
import es.tid.cosmos.platform.ial.libvirt.{LibVirtServerProperties, DomainProperties, LibVirtServer}

/** A JNA based libvirt server */
class JnaLibVirtServer(override val properties: LibVirtServerProperties) extends LibVirtServer {

  val domainId: Int = 101
  val domainName: String = domainId.toString

  private val conn = new Connect(properties.connectionChain)

  private val openVzDomainXmlDoc = {
    <domain type='openvz' id={domainId.toString}>
      <name>{domainName}</name>

      <!-- VZ driver replaces the VCPU value by the available physical CPUs when that first
           is greater than the second. We set 9999 to allocate all available CPUs to the domain.
           Unfortunately, this software will be obsolete before a regular server have thousand
           CPUs -->
      <vcpu>9999</vcpu>

      <!-- This property is not used by OpenVZ but required by libvirt. VZ assigns the memory
           specified in the config file. -->
      <memory unit='KiB'>0</memory>

      <os>
        <type>exe</type>
        <init>/sbin/init</init>
      </os>
    </domain>
  }

  def createDomain(): Future[DomainProperties] = JnaLibVirtServer.domainCreatorSequencer enqueue {
    destroyDomain().map(_ => mapDomain(blocking {
      conn.domainCreateXML(openVzDomainXmlDoc.toString(), 0)
    }))
  }

  def domain(): Future[DomainProperties] = future { blocking {
    try {
      mapDomain(conn.domainLookupByID(domainId))
    } catch {
      case ex: LibvirtException => throw new NoSuchElementException(
        s"there is no domain for ID $domainId in libvirt server ${properties.name}")
    }
  }}

  def isCreated(): Future[Boolean] = future { blocking {
    conn.domainLookupByID(domainId).isActive == 1
  }} recover {
    case _: LibvirtException => false
  }

  def destroyDomain(): Future[Unit] =
    (for {
      created <- isCreated() if (created)
      domain <- future { conn.domainLookupByID(domainId) }
      _ <- shutdownDomain(domain)
      _ <- future { blocking { domain.undefine() }}
      } yield ()
    ) recover {
      // domain not created, so drop the exception and return Unit
      case _: NoSuchElementException => ()
    }

  private def shutdownDomain(domain: Domain) : Future[Unit] =
    future { blocking { domain.shutdown() }} recover {
      // Raised if domain doesn't exist, just ignore
      case LibvirtError(code) => ()
    }

  private object LibvirtError {
    def unapply(ex: LibvirtException): Option[ErrorNumber] = Some(ex.getError.getCode)
  }

  private def mapDomain(domain: Domain): DomainProperties =
    new DomainProperties(
      uuid = UUID.nameUUIDFromBytes(domain.getUUID.map(_.toByte)),
      name = domain.getName,
      isActive = domain.isActive > 0,
      profile = properties.profile,
      hostname = properties.domainHostname,
      ipAddress = properties.domainIpAddress)
}

object JnaLibVirtServer {
  /*
   * This variable needs to be in a shared object to ensure that no repeated MAC addresses are assigned.
   */
  private val domainCreatorSequencer = new SequentialOperations
}

