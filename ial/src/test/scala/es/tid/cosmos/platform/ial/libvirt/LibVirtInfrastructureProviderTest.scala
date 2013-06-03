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
import scala.concurrent.{Future, Await}
import scala.concurrent.duration._

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
    val machines_> = infraProvider.createMachines("test_", MachineProfile.M, 3, action)
    machines_> must runUnder (timeout)
    machines_> must eventually(have length 3)
    machines_> must eventually(matchAll.withProfile(MachineProfile.M))
    action.bootstrappedMachines must have size (3)
  }

  it must "not create machines when unavailable" in {
    evaluating {
      infraProvider.createMachines("test_", MachineProfile.M, 30, action) must runUnder (timeout)
    } must produce [ResourceExhaustedException]
    action.bootstrappedMachines must be ('empty)
  }

  it must "not create machines when unavailable for requested profile" in {
    evaluating {
      infraProvider.createMachines("test_", MachineProfile.XL, 1, action) must runUnder (timeout)
    } must produce [ResourceExhaustedException]
    action.bootstrappedMachines must be ('empty)
  }

  it must "not create machines after several requests and resources exhausted" in {
    evaluating {
      for (i <- 0 to 3) {
        val machines_> = infraProvider.createMachines(s"test${i}_", MachineProfile.M, 3, action)
        machines_> must runUnder (timeout)
      }
    } must produce [ResourceExhaustedException]
  }

  it must "release machines" in {
    val machines = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 3, action), timeout)
    infraProvider.releaseMachines(machines) must runUnder (timeout)
  }

  it must "create machines after some are released" in {
    val machines1 = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 2, action), timeout)
    val machines2 = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 2, action), timeout)

    infraProvider.releaseMachines(machines1) must runUnder (timeout)
    infraProvider.releaseMachines(machines2) must runUnder (timeout)

    infraProvider.createMachines("test_", MachineProfile.M, 2, action) must runUnder (timeout)
    infraProvider.createMachines("test_", MachineProfile.M, 2, action) must runUnder (timeout)
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
