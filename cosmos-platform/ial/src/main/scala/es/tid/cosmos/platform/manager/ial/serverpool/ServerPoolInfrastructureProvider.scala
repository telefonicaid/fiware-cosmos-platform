package es.tid.cosmos.platform.manager.ial.serverpool

import scala.concurrent.{Promise, Future}
import scala.util.{Success, Failure, Try}

import es.tid.cosmos.platform.manager.ial._
import es.tid.cosmos.platform.manager.ial.MachineState

/**
 * @author apv
 */
class ServerPoolInfrastructureProvider(val dao: ServerPoolDao) extends InfrastructureProvider {

  def createMachines(namePrefix: String,
                     profile: MachineProfile.Value,
                     count: Int): Try[Seq[Future[MachineState]]] = {
    val machineIds = dao.getAvailableMachinesWith(_.profile == profile).take(count).map(_.id)
    if (machineIds.length < count) Failure(ResourceExhaustedException(count, machineIds.length))
    else {
      var machines: List[MachineState] = List()
      machineIds.zip(0 to machineIds.length).foreach {
        case (id, idx) => {
          dao.setMachineAvailability(id, available = false)
          machines = machines :+ dao.setMachineName(id, s"$namePrefix$idx").get
        }
      }
      Success(machines.map(m => Promise.successful(m).future))
    }
  }
}
