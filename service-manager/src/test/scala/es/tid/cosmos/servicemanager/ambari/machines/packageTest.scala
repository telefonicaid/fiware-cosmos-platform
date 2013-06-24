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

package es.tid.cosmos.servicemanager.ambari.machines

import scala.concurrent.Future

import org.mockito.BDDMockito._
import org.scalatest.OneInstancePerTest
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.platform.ial.MachineState
import es.tid.cosmos.servicemanager.ambari.rest.AmbariTestBase

class packageTest extends AmbariTestBase with OneInstancePerTest with MockitoSugar{

  val machineNameFunction: MachineState => String = state => state.name
  val (master, slave1, slave2) = (mock[MachineState], mock[MachineState], mock[MachineState])

  given(master.name).willReturn("master")
  given(slave1.name).willReturn("slave1")
  given(slave2.name).willReturn("slave2")

  "Calling masterAndSlaves" must "need at least one machine" in {
    evaluating (masterAndSlaves(Seq())) must produce[IllegalArgumentException]
  }

  it must "yield the master as slave as well when only one machine is given" in {
    masterAndSlaves(Seq(master)) must equal((master, Seq(master)))
  }

  it must "yield the first machine as master and the rest as slaves" +
    " when more than one machine is given" in {
    masterAndSlaves(Seq(master, slave1, slave2)) must equal((master, Seq(slave1, slave2)))
  }

  "Calling mapMaster" must "apply the given function the given machines' master" in {
    val machines_> = Future.successful(Seq(master, slave1, slave2))
    get(mapMaster(machines_>, machineNameFunction)) must equal("master")
  }

  "Calling mapSlaves" must "apply the given function the given machines' slaves" in {
    val machines_> = Future.successful(Seq(master, slave1, slave2))
    get(mapSlaves(machines_>, machineNameFunction)) must equal(Seq("slave1", "slave2"))
  }
}
