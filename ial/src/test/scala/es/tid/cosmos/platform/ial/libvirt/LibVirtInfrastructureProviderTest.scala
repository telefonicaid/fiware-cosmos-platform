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
import scala.language.postfixOps
import scalaz.syntax.validation._

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.{ExecutableValidation, PassThrough}
import es.tid.cosmos.common.scalatest.matchers.{FutureMatchers, ForAllMatcher}
import es.tid.cosmos.platform.ial._
import es.tid.cosmos.platform.ial.MachineProfile._

class LibVirtInfrastructureProviderTest extends FlatSpec with MustMatchers with FutureMatchers
    with OneInstancePerTest {

  implicit val noCondition = PassThrough
  val timeout: FiniteDuration = 5 seconds
  val infraProvider: LibVirtInfrastructureProvider = new LibVirtInfrastructureProvider(
        new FakeLibVirtDao, new FakeLibVirtServerFactory, "DUMMY-SSH-KEY")
  val action: TestBootstrapAction = new TestBootstrapAction

  "Libvirt Infra Provider" must "create machines when available" in {
    val machines_> = infraProvider.createMachines(noCondition, G1Compute, 3, action)
    machines_> must runUnder (timeout)
    machines_> must eventually(have length 3)
    machines_> must eventually(matchAll.withProfile(G1Compute))
    action.bootstrappedMachines must have size (3)
  }

  it must "not create machines when unavailable" in {
    mustNotCreateMachines(
      infraProvider.createMachines(noCondition, G1Compute, 30, action))
  }

  it must "not create machines when unavailable for requested profile" in {
    mustNotCreateMachines(
      infraProvider.createMachines(noCondition, HdfsMaster, 2, action))
  }

  it must "not fail to create machines if connecting to one of them fails" in {
    val serverFactory = new FakeLibVirtServerFactory
    var called = false
    def throwOnFirstInvocation(properties: LibVirtServerProperties) = {
      if (!called) {
        called = true
        throw new Error()
      } else {
        serverFactory(properties)
      }
    }
    val ial = new LibVirtInfrastructureProvider(
      new FakeLibVirtDao, throwOnFirstInvocation, "DUMMY-SSH-KEY")
    Await.result(ial.createMachines(noCondition, G1Compute, 2, action), timeout)
  }

  it must "not create machines after several requests and resources exhausted" in {
    for (i <- 0 to 2) {
      val machines_> = infraProvider.createMachines(noCondition, G1Compute, 3, action)
      machines_> must runUnder (timeout)
    }
    val machines_> = infraProvider.createMachines(noCondition, G1Compute, 3, action)
    machines_> must runUnder (timeout)
    machines_> must eventuallyFailWith [ResourceExhaustedException]
  }

  it must "release machines" in {
    val machines = Await.result(
      infraProvider.createMachines(noCondition, G1Compute, 3, action), timeout)
    infraProvider.releaseMachines(machines) must runUnder (timeout)
  }

  it must "create machines after some are released" in {
    val machines1 = Await.result(
      infraProvider.createMachines(noCondition, G1Compute, 2, action), timeout)
    val machines2 = Await.result(
      infraProvider.createMachines(noCondition, G1Compute, 2, action), timeout)

    infraProvider.releaseMachines(machines1) must runUnder (timeout)
    infraProvider.releaseMachines(machines2) must runUnder (timeout)

    infraProvider.createMachines(noCondition, G1Compute, 2, action) must runUnder (timeout)
    infraProvider.createMachines(noCondition, G1Compute, 2, action) must runUnder (timeout)
  }

  it must "provide available machine count for full availability" in {
    val count_> = infraProvider.availableMachineCount(G1Compute)
    count_> must runUnder(timeout)
    count_> must eventually(equal (5))
  }

  it must "provide available machine count when some are assigned" in {
    val machines_> = infraProvider.createMachines(noCondition, G1Compute, 2, action)
    machines_> must runUnder(timeout)

    val count_> = infraProvider.availableMachineCount(G1Compute)
    count_> must runUnder(timeout)
    count_> must eventually(equal (3))
  }

  it must "provide the machine state from assigned hostnames" in {
    val machines_> = for {
      createdMachines <- infraProvider.createMachines(noCondition, G1Compute, 2, action)
      assignedMachines <- infraProvider.assignedMachines(createdMachines.map(m => m.hostname))
    } yield assignedMachines

    machines_> must runUnder(timeout)
    machines_> must eventually(have size 2)
  }

  it must "provide the total number of managed machines according to profile filter and " +
    "regardless of their state or usage" in {
    infraProvider.machinePoolCount(_ => true) must be (9)
    infraProvider.machinePoolCount(_ == G1Compute) must be (5)
    infraProvider.machinePoolCount(_ == HdfsMaster) must be (1)
    infraProvider.machinePoolCount(_ == HdfsSlave) must be (3)
  }

  it must "accept creating machines but eventually deny it when pre-conditions are not met" in {
    val willFailCondition: ExecutableValidation = () => "Failed!".failureNel
    val beforeMachineCount_> = infraProvider.availableMachineCount(G1Compute)
    val machines_> = infraProvider.createMachines(willFailCondition, G1Compute, 2, action)
    machines_> must eventuallyFailWith[PreconditionsNotMetException]
    val afterMachineCount_> = infraProvider.availableMachineCount(G1Compute)
    val noMachinesWereAllocated_> = for {
      before <- beforeMachineCount_>
      after <- afterMachineCount_>
    } yield before == after
    noMachinesWereAllocated_> must eventually(be (true))
  }

  def mustNotCreateMachines(machines_> : Future[Seq[MachineState]]) {
    machines_> must runUnder (timeout)
    machines_> must eventuallyFailWith [ResourceExhaustedException]
    action.bootstrappedMachines must be ('empty)
  }

  object matchAll {
    def withProfile(profile: Value) = new ForAllMatcher[MachineState](
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
