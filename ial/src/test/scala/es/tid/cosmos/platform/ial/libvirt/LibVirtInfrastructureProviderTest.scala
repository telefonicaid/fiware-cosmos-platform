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

import scala.collection.concurrent
import scala.concurrent.duration._
import scala.concurrent.{Future, Await}
import scala.concurrent.ExecutionContext.Implicits.global

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.{FutureMatchers, ForAllMatcher}
import es.tid.cosmos.platform.ial.{Id, ResourceExhaustedException, MachineState, MachineProfile}

class LibVirtInfrastructureProviderTest extends FlatSpec with MustMatchers with FutureMatchers with BeforeAndAfter {

  val timeout: FiniteDuration = 1 second
  var infraProvider: LibVirtInfrastructureProvider = null
  var action: TestBootstrapAction = null

  before {
    infraProvider = new LibVirtInfrastructureProvider(
      new FakeLibVirtDao, new FakeLibVirtServerFactory, "DUMMY-SSH-KEY")
    action = new TestBootstrapAction
  }

  after {}

  "Libvirt Infra Provider" must "create machines when available" in {
    val machines_> = infraProvider.createMachines(MachineProfile.G1Compute, 3, action)
    machines_> must runUnder (timeout)
    machines_> must eventually(have length 3)
    machines_> must eventually(matchAll.withProfile(MachineProfile.G1Compute))
    action.bootstrappedMachines must have size (3)
  }

  it must "not create machines when unavailable" in {
    evaluating {
      infraProvider.createMachines(MachineProfile.G1Compute, 30, action) must runUnder (timeout)
    } must produce [ResourceExhaustedException]
    action.bootstrappedMachines must be ('empty)
  }

  it must "not create machines when unavailable for requested profile" in {
    evaluating {
      infraProvider.createMachines(MachineProfile.HdfsMaster, 2, action) must runUnder (timeout)
    } must produce [ResourceExhaustedException]
    action.bootstrappedMachines must be ('empty)
  }

  it must "not create machines after several requests and resources exhausted" in {
    evaluating {
      for (i <- 0 to 3) {
        val machines_> = infraProvider.createMachines(MachineProfile.G1Compute, 3, action)
        machines_> must runUnder (timeout)
      }
    } must produce [ResourceExhaustedException]
  }

  it must "release machines" in {
    val machines = Await.result(
      infraProvider.createMachines(MachineProfile.G1Compute, 3, action), timeout)
    infraProvider.releaseMachines(machines) must runUnder (timeout)
  }

  it must "create machines after some are released" in {
    val machines1 = Await.result(
      infraProvider.createMachines(MachineProfile.G1Compute, 2, action), timeout)
    val machines2 = Await.result(
      infraProvider.createMachines(MachineProfile.G1Compute, 2, action), timeout)

    infraProvider.releaseMachines(machines1) must runUnder (timeout)
    infraProvider.releaseMachines(machines2) must runUnder (timeout)

    infraProvider.createMachines(MachineProfile.G1Compute, 2, action) must runUnder (timeout)
    infraProvider.createMachines(MachineProfile.G1Compute, 2, action) must runUnder (timeout)
  }

  it must "provide available machine count for full availability" in {
    val count_> = infraProvider.availableMachineCount(MachineProfile.G1Compute)
    count_> must runUnder(timeout)
    count_> must eventually(equal (5))
  }

  it must "provide available machine count when some are assigned" in {
    val machines_> = infraProvider.createMachines(MachineProfile.G1Compute, 2, action)
    machines_> must runUnder(timeout)

    val count_> = infraProvider.availableMachineCount(MachineProfile.G1Compute)
    count_> must runUnder(timeout)
    count_> must eventually(equal (3))
  }

  it must "provide the machine state from assigned hostnames" in {
    val machines_> = for {
      createdMachines <- infraProvider.createMachines(MachineProfile.G1Compute, 2, action)
      assignedMachines <- infraProvider.assignedMachines(createdMachines.map(m => m.hostname))
    } yield assignedMachines

    machines_> must runUnder(timeout)
    machines_> must eventually(have size 2)
  }

  object matchAll {
    def withProfile(profile: MachineProfile.Value) = new ForAllMatcher[MachineState](
      s"profile $profile")(srv => srv.profile == profile)
  }

  class TestBootstrapAction extends Function[MachineState, Future[Unit]] {
    private val bootstrapped: concurrent.Map[Id[MachineState], MachineState] =
      new concurrent.TrieMap()

    def apply(state: MachineState): Future[Unit] = {
      val replaced: Option[MachineState] = bootstrapped.put(state.id, state)
      replaced must not be ('defined)
      Future.successful(())
    }

    def bootstrappedMachines = bootstrapped.keySet
  }
}
