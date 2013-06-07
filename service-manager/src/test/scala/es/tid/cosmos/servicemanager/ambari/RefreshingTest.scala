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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.Future._

import org.mockito.BDDMockito._
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.rest._
import es.tid.cosmos.platform.ial.InfrastructureProvider
import es.tid.cosmos.platform.ial.MachineState
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.rest.{
  ClusterProvisioner, AmbariTestBase, Service, Cluster}

class RefreshingTest extends AmbariTestBase with MockitoSugar {

  "A refreshable" must "refresh to no clusters when provisioner offers none" in new NoRefresh {
    val clusterIds = Seq()
    given(provisioner.listClusterNames).willReturn(successful(Seq()))
    get(refresh())
  }

  it must "not refresh when provisioner yields no new clusters" in new NoRefresh {
    val clusterIds = Seq(ClusterId("registeredCluster"))
    given(provisioner.listClusterNames).willReturn(successful(Seq("registeredCluster")))
    get(refresh())
  }

  it must "refresh cluster state to Running if all services are Started" in new ExpectRefresh {
    val clusterIds = Seq()
    def clusterName = "unregistered"
    given(service.state).willReturn("STARTED")
    def registerCluster(description: MutableClusterDescription) {
      checkClusterHasState(Running, description)
    }
    get(refresh())
  }

  it must "register only any unregistered clusters the provisioner yields upon a refresh" in
    new ExpectRefresh {
      val clusterIds = Seq(ClusterId("registered"))
      def clusterName = "unregistered"
      given(service.state).willReturn("STARTED")
      def registerCluster(description: MutableClusterDescription) {
        checkClusterHasState(Running, description)
      }
      get(refresh())
    }

  it must "must fail refreshing a cluster that does not stabilize within grace period" in
    new ExpectRefresh {
      val clusterIds = Seq()
      def clusterName = "unregistered"
      given(service.state).willReturn("INSTALLED")
      def registerCluster(description: MutableClusterDescription) {
        checkDescriptionInfo(description)
      }
      evaluating (get(refresh())) must produce [IllegalStateException]
    }

  it must "must refresh a cluster that stabilizes within grace period" in new ExpectRefresh {
    val clusterIds = Seq()
    def clusterName = "unregistered"
    given(service.state).willReturn("INSTALLED").willReturn("STARTED")
    def registerCluster(description: MutableClusterDescription) {
      checkDescriptionInfo(description)
      description.view.state must equal(Provisioning)
      get(description.deployment_>)
      Thread.sleep(50)
      description.view.state must equal(Running)
    }
    get(refresh())
  }

  trait BaseRefreshable extends Refreshing {
    val infrastructureProvider = mock[InfrastructureProvider]
    val provisioner = mock[ClusterProvisioner]
    val refreshGracePeriod: Int = 50
  }

  trait NoRefresh extends BaseRefreshable {
    def registerCluster(description: MutableClusterDescription) {
      fail("Should have not registered any cluster")
    }
  }

  trait ExpectRefresh extends BaseRefreshable {
    def clusterName: String
    val serviceName = "myService"
    val hostName = "myhost"
    val cluster = mock[Cluster]
    val service = mock[Service]
    val host = mock[Host]
    val cluster_machines_> = successful(Seq(mock[MachineState]))
    given(provisioner.listClusterNames).willReturn(successful(Seq(clusterName)))
    given(provisioner.getCluster(clusterName)).willReturn(successful(cluster))
    given(cluster.name).willReturn(clusterName)
    given(cluster.hostNames).willReturn(Seq(hostName))
    given(cluster.getHosts).willReturn(successful(Seq(host)))
    given(cluster.serviceNames).willReturn(Seq(serviceName))
    given(cluster.getService(serviceName)).willReturn(successful(service))
    given(host.getComponentNames).willReturn(Seq("NAMENODE"))
    given(infrastructureProvider.assignedMachines(Seq(hostName))).willReturn(cluster_machines_>)

    def checkDescriptionInfo(description: MutableClusterDescription) {
      description must (
        have ('id (ClusterId(clusterName))) and
          have ('name (clusterName)) and
          have ('size (1)) and
          have ('machines_> (cluster_machines_>))
        )
    }

    def checkClusterHasState(state: ClusterState, description: MutableClusterDescription) {
      checkDescriptionInfo(description)
      get(description.deployment_>)
      Thread.sleep(50)
      description.view.state must equal(state)
    }
  }
}
