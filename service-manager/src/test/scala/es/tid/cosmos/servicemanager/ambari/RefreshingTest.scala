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

import java.net.URI
import scala.annotation.tailrec
import scala.concurrent.Future
import scala.concurrent.Future._
import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits.global

import com.ning.http.client.Request
import dispatch.StatusCode
import org.mockito.BDDMockito._
import org.scalatest.mock.MockitoSugar
import org.scalatest.matchers.{MatchResult, BeMatcher}

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.ial.{InfrastructureProvider, MachineState}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.rest._

class RefreshingTest extends AmbariTestBase with MockitoSugar with FutureMatchers {
  "A refreshable" must "refresh no clusters when provisioner offers none" in new NoRefresh {
    val clusterIds = Seq()
    given(provisioner.listClusterNames).willReturn(successful(Seq()))
    get(refresh())
  }

  it must "not refresh when provisioner yields no new clusters" in new NoRefresh {
    val clusterIds = Seq(ClusterId("registeredCluster"))
    given(provisioner.listClusterNames).willReturn(successful(Seq("registeredCluster")))
    get(refresh())
  }

  it must "refresh cluster state to Running if all services are in their Running state" in
    new ExpectRefresh {
      val clusterIds = Seq()
      def clusterName = "unregistered"
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("STARTED")
      given(service3.state).willReturn("STARTED")
      def registerCluster(description: MutableClusterDescription) {
        checkDescriptionInfo(description)
      }
      get(refresh())
      finalState() must equal(Running)
    }

  it must "register only any unregistered clusters the provisioner yields upon a refresh" in
    new ExpectRefresh {
      val clusterIds = Seq(ClusterId("registered"))
      def clusterName = "unregistered"
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("STARTED")
      given(service3.state).willReturn("STARTED")
      def registerCluster(description: MutableClusterDescription) {
        checkDescriptionInfo(description)
      }
      get(refresh())
      finalState() must equal(Running)
    }

  it must "must fail refreshing a cluster when an unknown service does " +
    "not stabilize within grace period" in new ExpectRefresh {
      val clusterIds = Seq()
      def clusterName = "unregistered"
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("STARTED")
      given(service3.state).willReturn("INSTALLED")
      def registerCluster(description: MutableClusterDescription) {
        checkDescriptionInfo(description)
      }
      evaluating (get(refresh())) must produce [IllegalStateException]
      finalState() must be (failedWithIllegalState)
    }

  it must "must fail refreshing a cluster when a known service " +
    "does not stabilize within grace period" in new ExpectRefresh {
      val clusterIds = Seq()
      def clusterName = "unregistered"
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("INSTALLED")
      given(service3.state).willReturn("STARTED")
      def registerCluster(description: MutableClusterDescription) {
        checkDescriptionInfo(description)
      }
      evaluating (get(refresh())) must produce [IllegalStateException]
      finalState() must be (failedWithIllegalState)
    }

  it must "refresh a cluster that stabilizes within grace period" in new ExpectRefresh {
    val clusterIds = Seq()
    def clusterName = "unregistered"
    given(service1.state).willReturn("INSTALLED")
    given(service2.state).willReturn("INSTALLED").willReturn("STARTED")
    given(service3.state).willReturn("INSTALLED").willReturn("INSTALLED").willReturn("STARTED")
    def registerCluster(description: MutableClusterDescription) {
      checkDescriptionInfo(description)
      description.view.state must equal(Provisioning)
    }
    get(refresh())
    finalState() must equal(Running)
  }

  it must "refresh a cluster in Terminated state if accessing its info returns 404 not found" in
    new ExpectRefresh {
      val clusterIds = Seq()
      def clusterName = "unregistered"
      val notFound: Future[Service] =
        failed(RequestException(mock[Request], "errorMessage", StatusCode(404)))
      given(cluster.getService(unknownService)).willReturn(notFound)
      def registerCluster(description: MutableClusterDescription) {
        checkDescriptionInfo(description)
      }
      get(refresh())
      finalState(attempts = 1 to 5) must equal (Terminated)
    }

  trait BaseRefreshable extends Refreshing {
    val infrastructureProvider = mock[InfrastructureProvider]
    val provisioner = mock[ClusterProvisioner]
    val refreshGracePeriod = 3000.milliseconds
    val clientOnlyDescription = new ServiceDescription {
      val components: Seq[ComponentDescription] = Seq(
        ComponentDescription("", isMaster = true, isClient = true),
        ComponentDescription("", isMaster = false, isClient = true))
      val name: String = "ClientService"
    }
    val normalDescription = new ServiceDescription {
      val components: Seq[ComponentDescription] = Seq(
        ComponentDescription("", isMaster = true, isClient = true),
        ComponentDescription("", isMaster = true, isClient = false))
      val name: String = "NormalService"
    }
    val serviceDescriptions = Seq(clientOnlyDescription, normalDescription)
  }

  trait NoRefresh extends BaseRefreshable {
    def registerCluster(description: MutableClusterDescription) {
      fail("Should have not registered any cluster")
    }
  }

  trait ExpectRefresh extends BaseRefreshable {
    def clusterName: String
    private var descriptionHandle: MutableClusterDescription = null
    val unknownService = "myUnknownService"
    val hostName = "myhost"
    val cluster = mock[Cluster]
    val service1 = mock[Service]
    val service2 = mock[Service]
    val service3 = mock[Service]
    val services = Seq(service1, service2, service3)
    val host = mock[Host]
    val cluster_machines_> = successful(Seq(mock[MachineState]))
    given(provisioner.listClusterNames).willReturn(successful(Seq(clusterName)))
    given(provisioner.getCluster(clusterName)).willReturn(successful(cluster))
    given(cluster.name).willReturn(clusterName)
    given(cluster.hostNames).willReturn(Seq(hostName))
    given(cluster.getHosts).willReturn(successful(Seq(host)))
    given(service1.name).willReturn(serviceDescriptions.head.name)
    given(service2.name).willReturn(serviceDescriptions.last.name)
    given(service3.name).willReturn(unknownService)
    val serviceNames = services.map(_.name)
    given(cluster.serviceNames).willReturn(serviceNames)
    services.foreach(srv => given(cluster.getService(srv.name)).willReturn(successful(srv)))
    given(host.getComponentNames).willReturn(Seq("NAMENODE"))
    given(host.name).willReturn(hostName)
    given(infrastructureProvider.assignedMachines(Seq(hostName))).willReturn(cluster_machines_>)

    def checkDescriptionInfo(description: MutableClusterDescription) {
      description must (
        have ('id (ClusterId(clusterName))) and
          have ('name (clusterName)) and
          have ('size (1)) and
          have ('machines_> (cluster_machines_>))
        )
      description.deployment_>.onComplete(_ => descriptionHandle = description)
      description.nameNode_> must eventually (be (new URI(s"hdfs://$hostName:50070")))
    }

    /**
     * Wait for the generated description to obtain the final cluster state and return that state.
     *
     * @param attempts (optional). Use it for tests of Terminated state where the refresher
     *                 will temporarily switch from Provisioning to Running before finally resolving
     *                 to Terminated. For each entry in attempts and when the state becomes Running
     *                 it will retry reading the state until it changes from Running or it runs out
     *                 of attempts.
     * @return
     */
    @tailrec
    final def finalState(attempts: Seq[Any] = Seq()): ClusterState =  {
      if (descriptionHandle == null) { Thread.sleep(50); finalState() }
      else descriptionHandle.state match {
        case Provisioning | Terminating => Thread.sleep(50); finalState()
        case Running =>
          if (!attempts.isEmpty) { Thread.sleep(500); finalState(attempts.tail) }
          else Running
        case other@_ => other
      }
    }
  }

  def failedWithIllegalState: BeMatcher[ClusterState] = new BeMatcher[ClusterState] {
    def apply(state: ClusterState): MatchResult = state match {
      case Failed(e:IllegalStateException) =>
        MatchResult(matches = true, "", "Expected Failed(IllegalStateException(...) state")
      case oops@_ =>
        MatchResult(matches = false,
          s"Unexpected state: Expected Failed(IllegalStateException(...) but got $oops", "")
    }
  }
}
