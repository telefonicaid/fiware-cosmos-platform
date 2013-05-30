package es.tid.cosmos.platform.ial.libvirt

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.platform.ial._

/**
 * An infrastructure provider implemented on top of libvirt
 */
class LibVirtInfrastructureProvider(
    val dao: LibVirtDao,
    val libvirtServerFactory: LibVirtServerProperties => LibVirtServer)
    extends InfrastructureProvider {

  def createMachines(
      namePrefix: String,
      profile: MachineProfile.Value,
      numberOfMachines: Int): Future[Seq[MachineState]] = {

    def takeServers(servers: Seq[LibVirtServer]) = {
      val available = servers.length
      if (available < numberOfMachines)
        throw ResourceExhaustedException(profile.toString, numberOfMachines, available)
      else
        servers.take(numberOfMachines)
    }

    for {
      servers <- availableServers(profile)
      machines <- createMachines(takeServers(servers))
    } yield machines
  }

  /**
   * Release the machines so that its resources can be reused in further createMachine requests
   *
   * @param machines The set of machines to be released.
   * @return         a future which terminates once the release has completed
   */
  def releaseMachines(machines: Seq[MachineState]): Future[Unit] = {
    for {
      servers <- serversForMachines(machines)
    } yield Future.sequence(servers.map(srv => srv.destroyDomain()))
  }

  private def serversForMachines(machines: Seq[MachineState]): Future[Seq[LibVirtServer]] = Future {
      for {
        machine <- machines
        server <- dao.libVirtServers if (server.domainHostname == machine.hostname)
      } yield libvirtServerFactory(server)
  }

  val rootSshKey: String = "FIXME"


  private def createMachines(
      servers: Seq[LibVirtServer]): Future[Seq[MachineState]] = {
    Future.sequence(servers.map(srv => srv.createDomain().map(dom => domainToMachineState(dom))))
  }

  private def availableServers(profile: MachineProfile.Value): Future[Seq[LibVirtServer]] = {
    val servers = dao.libVirtServers
      .filter(_.profile == profile)
      .map(prop => libvirtServerFactory(prop))
    Future.sequence(servers.map(srv => for (created <- srv.isCreated()) yield (srv, created)))
      .map(_.collect { case (srv, false) => srv})
  }

  private def domainToMachineState(domain : DomainProperties): MachineState =
    new MachineState(
      Id(domain.uuid),
      domain.name,
      MachineProfile.M,
      domainStatus(domain),
      domain.hostname,
      domain.ipAddress)

  private def domainStatus(dom: DomainProperties) : MachineStatus.Value =
    if (dom.isActive) MachineStatus.Running else MachineStatus.Provisioning
}
