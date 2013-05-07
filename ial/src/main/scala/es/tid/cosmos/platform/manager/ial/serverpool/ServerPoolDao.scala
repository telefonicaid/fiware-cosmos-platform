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

package es.tid.cosmos.platform.manager.ial.serverpool

import es.tid.cosmos.platform.manager.ial._

/**
 * A data access object for server pool state.
 *
 * @author apv
 */
trait ServerPoolDao {
  /**
   * Obtain the list of available machines which satisfies the given predicate.
   *
   * @param f the predicate the available machines must satisfy
   * @return the sequence of machines which are available and satisfy the predicate
   */
  def availableMachinesWith(f: MachineState => Boolean) : Seq[MachineState]

  /**
   * Obtain the list of available machines.
   *
   * @return a sequence of available machines
   */
  def availableMachines: Seq[MachineState] = availableMachinesWith(m => true)

  /**
   * Get the status of the machine with the given identifier.
   *
   * @param machineId the identifier of the machine whose status is to be obtained
   * @return the machine state
   */
  def machine(machineId: Id[MachineState]): Option[MachineState]

  /**
   * Set the availability of the machine with the given identifier.
   *
   * @param machineId the identifier of the machine whose availability is to be modified
   * @param available the new availability of the machine (true is available, false is unavailable)
   * @return the new state of the machine
   */
  def setMachineAvailability(machineId: Id[MachineState], available: Boolean): Option[MachineState]

  /**
   * Set the name of the machine with the given identifier.
   *
   * @param machineId the identifier of the machine whose name is to be modified
   * @param name the new name of the machine
   * @return the new state of the machine
   */
  def setMachineName(machineId: Id[MachineState], name: String): Option[MachineState]
}
