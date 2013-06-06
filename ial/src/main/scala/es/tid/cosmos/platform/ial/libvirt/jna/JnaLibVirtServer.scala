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

package es.tid.cosmos.platform.ial.libvirt.jna

import java.util.{NoSuchElementException, UUID}

import scala.concurrent.{Future, future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.{Try, Random}

import org.libvirt.{LibvirtException, Domain, Connect}
import org.libvirt.Error.ErrorNumber

import es.tid.cosmos.platform.ial.libvirt.{LibVirtServerProperties, DomainProperties, LibVirtServer}

/**
 * A JNA based libvirt server
 */
case class JnaLibVirtServer(val properties: LibVirtServerProperties) extends LibVirtServer {

  val domainId: Int = 101
  val domainName: String = domainId.toString

  private val conn = new Connect(properties.connectionChain)

  private val openVzDomainXmlDoc = {
    <domain type='openvz' id={domainId.toString}>
      <name>{domainName}</name>
      <vcpu>{properties.numberOfCpus}</vcpu>
      <memory unit='KiB'>0</memory>
      <os>
        <type>exe</type>
        <init>/sbin/init</init>
      </os>
      <devices>
        <filesystem type='template'>
          <source name={properties.domainTemplate}/>
          <target dir='/'/>
          <space_hard_limit unit='GiB'>{properties.domainFilesystemQuota}</space_hard_limit>
          <space_soft_limit unit='GiB'>{properties.domainFilesystemQuota}</space_soft_limit>
        </filesystem>
        <interface type='bridge'>
          <mac address={randomMacAddress}/>
          <source bridge={properties.bridgeName}/>
        </interface>
      </devices>
    </domain>
  }

  def createDomain(): Future[DomainProperties] = destroyDomain().map(_ =>
    mapDomain(conn.domainCreateXML(openVzDomainXmlDoc.toString(), 0))
  )

  def isCreated(): Future[Boolean] = future {
    conn.listDomains().filter(dom => dom == domainId).length == 1
  }

  def destroyDomain(): Future[Unit] =
    (for {
      created <- isCreated() if (created)
      domain <- future { conn.domainLookupByID(domainId) }
      _ <- shutdownDomain(domain)
      _ <- future { domain.undefine() }
      } yield ()
    ) recover {
      // domain not created, so drop the exception and return Unit
      case _: NoSuchElementException => ()
    }

  private def shutdownDomain(domain: Domain) : Future[Unit] =
    future { domain.shutdown() } recover {
      // Raised if domain doesn't exist, just ignore
      case LibvirtError(code) => ()
    }

  private object LibvirtError {
    def unapply(ex: LibvirtException): Option[ErrorNumber] = Some(ex.getError.getCode)
  }

  private def randomMacAddress = {
    val address = new Array[Byte](6)
    Random.nextBytes(address)
    removeMulticastBit(address)
    setLocallyAdministeredBit(address)
    address.map(b => (b.toInt & 0x000000ff).toHexString).mkString(":")
  }

  private def removeMulticastBit(macAddress: Array[Byte]) {
    macAddress.update(0, (macAddress.head & 0xfe).toByte)
  }

  private def setLocallyAdministeredBit(macAddress: Array[Byte]) {
    macAddress.update(0, (macAddress.head | 0x02).toByte)
  }

  private def mapDomain(domain: Domain): DomainProperties =
    new DomainProperties(
      uuid = UUID.nameUUIDFromBytes(domain.getUUID.map(_.toByte)),
      name = domain.getName,
      isActive = domain.isActive > 0,
      hostname = properties.domainHostname,
      ipAddress = properties.domainIpAddress)
}
