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

package es.tid.cosmos.servicemanager.clusters

import java.net.URI

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ClusterUser

class ClusterDescriptionTest extends FlatSpec with MustMatchers {

  "A cluster description" must "have expected size 0 when cluster in Terminated state" in {
    withMachineInfoForState(Terminated).expectedSize must be (0)
    withoutMachineInfoForState(Terminated).expectedSize must be (0)
  }

  it must "have expected size equal to the requested size when in Provisioning state" in {
    val withMachines = withMachineInfoForState(Provisioning)
    withMachines.expectedSize must equal(withMachines.size)
    withMachines.expectedSize must be (10)
    
    val withoutMachines = withoutMachineInfoForState(Provisioning)
    withoutMachines.expectedSize must equal(withoutMachines.size)
    withoutMachines.expectedSize must be (10)
  }

  it must "have expected size equal to the requested size when in Running state" in {
    val withMachines = withMachineInfoForState(Running)
    withMachines.expectedSize must equal(withMachines.size)
    withMachines.expectedSize must be (10)

    val withoutMachines = withoutMachineInfoForState(Running)
    withoutMachines.expectedSize must equal(withoutMachines.size)
    withoutMachines.expectedSize must be (10)
  }

  it must "have expected size equal to the requested size when in Terminating state" in {
    val withMachines = withMachineInfoForState(Terminating)
    withMachines.expectedSize must equal(withMachines.size)
    withMachines.expectedSize must be (10)

    val withoutMachines = withoutMachineInfoForState(Terminating)
    withoutMachines.expectedSize must equal(withoutMachines.size)
    withoutMachines.expectedSize must be (10)
  }

  it must "have expected size equal to the number of registered machines when in Failed state" in {
    val withMachines = withMachineInfoForState(Failed("cluster blew up"))
    withMachines.expectedSize must equal(withMachines.machines.size)
    withMachines.expectedSize must be (2)

    val withoutMachines = withoutMachineInfoForState(Failed("cluster blew up"))
    withoutMachines.expectedSize must equal(withoutMachines.machines.size)
    withoutMachines.expectedSize must be (0)
  }

  def withMachineInfoForState(state: ClusterState) = ImmutableClusterDescription(
    id = ClusterId(),
    name = "myCluster",
    size = 10,
    state,
    nameNode = Some(new URI("someURI")),
    master = Some(HostDetails("master", "0.0.0.1")),
    slaves = Seq(HostDetails("slave", "0.0.0.2")),
    users = Some(Set(ClusterUser("jsmith", "a public key")))
  )
  
  def withoutMachineInfoForState(state: ClusterState) = ImmutableClusterDescription(
    id = ClusterId(),
    name = "myCluster",
    size = 10,
    state,
    nameNode = None,
    master = None,
    slaves = Seq.empty,
    users = None
  )
}
