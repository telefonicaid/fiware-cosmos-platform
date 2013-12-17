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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

trait LibVirtServer {

  /** Obtain the server properties. */
  def properties(): LibVirtServerProperties

  /** Create a new libvirt domain.
    *
    * @return the future properties of the newly created domain
    */
  def createDomain(): Future[DomainProperties]

  /** Get the existing domain for this server.
    *
    * If there is no such domain, a NoSuchElementException is encapsulated as error in the
    * resulting future.
    *
    * @return the properties of the existing domain, or NoSuchElementException error if there is
    *         no domain for this server
    */
  def domain(): Future[DomainProperties]

  /** Indicates whether the domain is created */
  def isCreated(): Future[Boolean]

  /** Destroy the existing domain.
    *
    * @return the future representing the destruction process
    */
  def destroyDomain(): Future[Unit]
}

object LibVirtServer {

  case class PlacementException(requested: Int, available: Int) extends RuntimeException(
    s"cannot place libvirt servers: requested $requested but only $available available"
  )

  /** Obtain as many servers as indicated trying to place as many of them in the same rack.
    *
    * If no enough servers are available, a PlacementException is encapsulated by the returned
    * future.
    */
  def placeServers(servers: Seq[LibVirtServer], howMany: Int): Future[Seq[LibVirtServer]] = {
    for {
      availServers <- availableServers(servers)
    } yield {
      val availServersCount = availServers.size
      if (availServers.size < howMany) {
        throw PlacementException(howMany, availServersCount)
      } else {
        val groups = availServers.groupBy(_.properties().rack)
        val sortedServers = groups.values.toSeq.sortWith((s1, s2) => s1.size > s2.size).flatten
        sortedServers.take(howMany)
      }
    }
  }

  /** Obtain a sequence of available servers. */
  def availableServers(servers: Seq[LibVirtServer]): Future[Seq[LibVirtServer]] = {
    def availableServer(srv: LibVirtServer): Future[Option[LibVirtServer]] =
      for (created <- srv.isCreated()) yield if (created) None else Some(srv)
    Future.sequence(servers.map(availableServer)).map(_.flatten)
  }
}
