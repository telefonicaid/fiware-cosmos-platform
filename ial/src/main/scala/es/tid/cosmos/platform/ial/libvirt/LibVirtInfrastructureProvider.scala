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
import scala.concurrent.{Future, future}

import es.tid.cosmos.platform.ial._

/**
 * An infrastructure provider implemented on top of libvirt
 */
class LibVirtInfrastructureProvider(
    val dao: LibVirtDao,
    val libvirtServerFactory: LibVirtServerProperties => LibVirtServer,
    val rootPrivateSshKey: String)
  extends InfrastructureProvider {

  def createMachines(
      namePrefix: String,
      profile: MachineProfile.Value,
      numberOfMachines: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]] = {

    def takeServers(servers: Seq[LibVirtServer]) = {
      val available = servers.length
      if (available < numberOfMachines)
        throw ResourceExhaustedException(profile.toString, numberOfMachines, available)
      else
        servers.take(numberOfMachines)
    }

    for {
      servers <- availableServers(profile)
      machines <- createMachines(bootstrapAction, takeServers(servers))
    } yield machines
  }

  /**
   * Release the machines so that its resources can be reused in further createMachine requests
   *
   * @param machines The set of machines to be released.
   * @return         a future which terminates once the release has completed
   */
  def releaseMachines(machines: Seq[MachineState]): Future[Unit] =
    for {
      servers <- serversForMachines(machines)
      _ <- Future.sequence(servers.map(srv => srv.destroyDomain()))
    } yield ()

  private def serversForMachines(machines: Seq[MachineState]): Future[Seq[LibVirtServer]] = future {
    for {
      machine <- machines
      server <- dao.libVirtServers if (server.domainHostname == machine.hostname)
    } yield libvirtServerFactory(server)
  }

  private def createMachines(
      bootstrapAction: MachineState => Future[Unit],
      servers: Seq[LibVirtServer]): Future[Seq[MachineState]] = Future.sequence(servers.map(srv =>
    for {
      domain <- srv.createDomain()
      state = domainToMachineState(domain)
      _      <- bootstrapAction(state)
    } yield state
  ))

  private def availableServers(profile: MachineProfile.Value): Future[Seq[LibVirtServer]] = {

    def availableServer(srv: LibVirtServer): Future[Option[LibVirtServer]] =
      for (created <- srv.isCreated()) yield if (created) None else Some(srv)

    val servers = for {
      serverProps <- dao.libVirtServers if (serverProps.profile == profile)
    } yield libvirtServerFactory(serverProps)
    Future.sequence(servers.map(availableServer)).map(_.flatten)
  }

  private def domainToMachineState(domain : DomainProperties): MachineState =
    new MachineState(
      Id(domain.uuid),
      domain.name,
      MachineProfile.M,
      domainStatus(domain),
      domain.hostname,
      domain.ipAddress)

  private def domainStatus(dom: DomainProperties): MachineStatus.Value =
    if (dom.isActive) MachineStatus.Running else MachineStatus.Provisioning
}
