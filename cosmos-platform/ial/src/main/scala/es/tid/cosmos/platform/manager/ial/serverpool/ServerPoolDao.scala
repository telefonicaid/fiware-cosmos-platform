package es.tid.cosmos.platform.manager.ial.serverpool

import es.tid.cosmos.platform.manager.ial._

private case class ServerPoolDaoInitException(msg: String, cause: Throwable)
    extends Exception(msg, cause)

/**
 * A data access object for server pool state.
 *
 * @author apv
 */
trait ServerPoolDao {

    def getAvailableMachinesWith(f: MachineState => Boolean) : Seq[MachineState]
    def getAvailableMachines: Seq[MachineState] = getAvailableMachinesWith(m => true)
    def getMachine(machineId: Id[MachineState]): Option[MachineState]
    def setMachineAvailability(machineId: Id[MachineState], available: Boolean): Option[MachineState]
}
