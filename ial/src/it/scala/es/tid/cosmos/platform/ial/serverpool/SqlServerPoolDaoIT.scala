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

package es.tid.cosmos.platform.ial.serverpool

import scala.Some

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.ial.{MachineState, MachineProfile, Id}

/**
 * @author apv
 */
class SqlServerPoolDaoIT extends FlatSpec with ServerPoolSqlTest with MustMatchers {

  "The SQL server pool DAO" must "list available machines" in {
      val machines = dao.availableMachines
      machines must have size (5)
      machines must containMachines(availableMachines)
  }

  it must "list available machines of given profile" in {
      val machines = dao.availableMachinesWith(_.profile == MachineProfile.M)
      machines must have size (2)
      machines must containMachines(List(availableMachines(1), availableMachines(2)))
  }

  it must "not list assigned machines as available" in {
      val machines = dao.availableMachines
      machines must have size (5)
      machines must not (containMachines(assignedMachines))
  }

  it must "get existing machine" in {
      val machines = dao.availableMachines
      val machine = dao.machine(machines(0).id)
      machine must be (Some(machines(0)))
  }

  it must "not get nonexistent machine" in {
      val machine = dao.machine(Id[MachineState])
      machine must be (None)
  }

  it must "mark machine as assigned" in {
      val machineId = dao.availableMachines(0).id
      dao.setMachineAvailability(machineId, available = false)
      dao.availableMachines must not (containMachine(machineId))
  }

  it must "mark machine as available" in {
      val machineId = dao.availableMachines(0).id
      dao.setMachineAvailability(machineId, available = false)
      dao.availableMachines must not (containMachine(machineId))
      dao.setMachineAvailability(machineId, available = true)
      dao.availableMachines must containMachine(machineId)
  }

  it must "set machine name" in {
    val machineId = dao.availableMachines(0).id
    val machine = dao.setMachineName(machineId, "foobar")
    machine must be ('defined)
    machine.get.name must be ("foobar")
    val m = dao.machine(machineId)
    m must be ('defined)
    m.get.name must be ("foobar")
  }

}
