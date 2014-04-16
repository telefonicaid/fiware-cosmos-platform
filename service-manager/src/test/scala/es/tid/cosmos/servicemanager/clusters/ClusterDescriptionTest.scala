/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager.clusters

import java.net.URI

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}

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
    id = ClusterId.random(),
    name = ClusterName("myCluster"),
    size = 10,
    state,
    nameNode = Some(new URI("someURI")),
    master = Some(HostDetails("master", "0.0.0.1")),
    slaves = Seq(HostDetails("slave", "0.0.0.2")),
    users = Some(Set(ClusterUser("jsmith", "a public key"))),
    services = Set("ServiceA", "ServiceB"),
    blockedPorts = Set.empty
  )

  def withoutMachineInfoForState(state: ClusterState) = ImmutableClusterDescription(
    id = ClusterId.random(),
    name = ClusterName("myCluster"),
    size = 10,
    state,
    nameNode = None,
    master = None,
    slaves = Seq.empty,
    users = None,
    services = Set("ServiceA", "ServiceB"),
    blockedPorts = Set.empty
  )
}
