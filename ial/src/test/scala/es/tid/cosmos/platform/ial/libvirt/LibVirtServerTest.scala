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

import org.scalatest.{Inside, FlatSpec}
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.ial.MachineProfile

class LibVirtServerTest extends FlatSpec with MustMatchers with FutureMatchers with Inside {

  val dao = new FakeLibVirtDao
  val propsOfComputeServers = dao.libVirtServers.filter(_.profile == MachineProfile.G1Compute)
  val computeServers: Seq[LibVirtServer] = propsOfComputeServers.map(new FakeLibVirtServer(_))

  val computeServersWithOneUnavailableInRack1 = propsOfComputeServers.map { props =>
    new FakeLibVirtServer(props, domainCreated = props.name == "andromeda05")
  }

  "LibVirtServer" must "place all servers on the same rack if that rack is fully available" in {
    val servers_> = LibVirtServer.placeServers(computeServers, 3)

    servers_> must eventually(havePlacedInRack("rack01", 3) and havePlacedInRack("rack02", 0))
  }

  it must "place all servers on the same rack if that rack is partially available but have enough space" in {
    val servers_> = LibVirtServer.placeServers(computeServersWithOneUnavailableInRack1, 2)

    servers_> must eventually(havePlacedInRack("rack01", 2) and havePlacedInRack("rack02", 0))
  }

  it must "place servers by rack availability if racks are fully available but servers not fit in just one" in {
    val servers_> = LibVirtServer.placeServers(computeServers, 4)

    servers_> must eventually(havePlacedInRack("rack01", 3) and havePlacedInRack("rack02", 1))
  }

  it must "place servers by rack availability if racks are partially available but servers not fit in just one" in {
    val servers_> = LibVirtServer.placeServers(computeServersWithOneUnavailableInRack1, 4)

    servers_> must eventually(havePlacedInRack("rack01", 2) and havePlacedInRack("rack02", 2))
  }

  it must "not place servers when required more than available" in {
    LibVirtServer.placeServers(
      computeServers, 6
    ) must eventuallyFailWith[LibVirtServer.PlacementException]
  }

  def havePlacedInRack(rack: String, amount: Int): Matcher[Seq[LibVirtServer]] =
    new Matcher[Seq[LibVirtServer]] {

      def apply(left: Seq[LibVirtServer]): MatchResult = {
        val groups = left.groupBy(_.properties().rack)
        val groupSize = groups.getOrElse(rack, Seq.empty).size
        MatchResult(
          matches = groupSize == amount,
          failureMessage = s"placement put $groupSize in rack $rack, but $amount was expected",
          negatedFailureMessage = s"placement put $amount in rack $rack, which was not expected"
        )
      }
    }
}
