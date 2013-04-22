package es.tid.cosmos.scalarest

import scala.concurrent._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.Try

import es.tid.cosmos.platform.manager.ial._
import es.tid.cosmos.platform.manager.ial.MachineState

/**
 * @author sortega
 */
class FakeInfrastructureProvider extends InfrastructureProvider {

  val machine = new MachineState(
    id = new Id[MachineState]("cosmosLocalID"),
    name = "cosmos.local",
    profile = MachineProfile.XS,
    status = MachineStatus.Running,
    hostname = "cosmos.local",
    ipAddress = "192.168.50.4")

  val machineFuture: Future[MachineState] = Future { machine }

  override def createMachines(namePrefix: String, profile: MachineProfile.Value, count: Int):
    Try[Seq[Future[MachineState]]] = Try(List(machineFuture))
}
