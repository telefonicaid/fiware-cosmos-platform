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

import scala.concurrent.Await
import scala.concurrent.duration.Duration

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.{ForAllMatcher, ContainSomeMatcher}
import es.tid.cosmos.platform.ial.{ResourceExhaustedException, MachineState, MachineProfile}

class LibVirtInfrastructureProviderTest extends FlatSpec with MustMatchers with BeforeAndAfter {

  var infraProvider: LibVirtInfrastructureProvider = null

  before {
    infraProvider = new LibVirtInfrastructureProvider(
      new FakeLibVirtDao, new FakeLibVirtServerFactory)
  }

  after {}

  "Libvirt Infra Provider" must "create machines when available" in {
    val machines = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 3), Duration.Inf)
    machines must have length 3
    machines must matchAll.withProfile(MachineProfile.M)
  }

  it must "not create machines when unavailable" in {
    evaluating {
      Await.result(infraProvider.createMachines("test_", MachineProfile.M, 30), Duration.Inf)
    } must produce [ResourceExhaustedException]
  }

  it must "not create machines when unavailable for requested profile" in {
    evaluating {
      Await.result(infraProvider.createMachines("test_", MachineProfile.XL, 1), Duration.Inf)
    } must produce [ResourceExhaustedException]
  }

  it must "not create machines after several requests and resources exhausted" in {
    evaluating {
      Await.result(infraProvider.createMachines("test0_", MachineProfile.M, 3), Duration.Inf)
      Await.result(infraProvider.createMachines("test1_", MachineProfile.M, 3), Duration.Inf)
      Await.result(infraProvider.createMachines("test2_", MachineProfile.M, 3), Duration.Inf)
      Await.result(infraProvider.createMachines("test3_", MachineProfile.M, 3), Duration.Inf)
    } must produce [ResourceExhaustedException]
  }

  it must "release machines" in {
    val machines = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 3), Duration.Inf)
    Await.result(infraProvider.releaseMachines(machines), Duration.Inf)
  }

  it must "create machines after some are released" in {
    val machines1 = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 2), Duration.Inf)
    val machines2 = Await.result(
      infraProvider.createMachines("test_", MachineProfile.M, 2), Duration.Inf)

    Await.result(infraProvider.releaseMachines(machines1), Duration.Inf)
    Await.result(infraProvider.releaseMachines(machines2), Duration.Inf)

    Await.result(infraProvider.createMachines("test_", MachineProfile.M, 2), Duration.Inf)
    Await.result(infraProvider.createMachines("test_", MachineProfile.M, 2), Duration.Inf)
  }

  object matchAll {
    def withProfile(profile: MachineProfile.Value) = new ForAllMatcher[MachineState](
      s"profile $profile")(srv => srv.profile == profile)
  }
}
