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
        val machines = dao.getAvailableMachinesWith(_.profile == profile).take(count).toList
        if (machines.length < count) Failure(ResourceExhaustedException(count, machines.length))
        else Success(machines.map(m => Promise.successful(m).future))
    }
}
