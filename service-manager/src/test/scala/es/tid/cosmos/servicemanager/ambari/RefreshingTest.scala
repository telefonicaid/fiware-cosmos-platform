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
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.ambari.configuration.NoConfigurationContribution

class RefreshingTest extends AmbariTestBase with MockitoSugar with FutureMatchers {

  val TestTimeout = 5 seconds

  "A refreshable" must "refresh no clusters when provisioner offers none" in new NoRefresh {
    val clusterIds = Seq()
    given(provisioner.listClusterNames).willReturn(successful(Seq()))
    refresh() must runUnder(TestTimeout)
  }

  it must "not refresh when provisioner yields no new clusters" in new NoRefresh {
    val clusterIds = Seq(ClusterId("registeredCluster"))
    given(provisioner.listClusterNames).willReturn(successful(Seq("registeredCluster")))
    refresh() must runUnder(TestTimeout)
  }

  it must "refresh cluster state to Running if all services are in their Running state" in
    new ExpectRefresh("unregistered") {
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("STARTED")
      given(service3.state).willReturn("STARTED")
      refresh() must runUnder(TestTimeout)
      stateIsReached(Running)
    }

  it must "register only any unregistered clusters the provisioner yields upon a refresh" in
    new ExpectRefresh("unregistered", knownClusters = Seq(ClusterId("registered"))) {
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("STARTED")
      given(service3.state).willReturn("STARTED")
      refresh() must runUnder(TestTimeout)
      stateIsReached(Running)
    }

  it must "fail refreshing a cluster when a unknown service does not stabilize within " +
    "grace period" in new ExpectRefresh("unregistered") {
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("STARTED")
      given(service3.state).willReturn("INSTALLED")
      refresh() must runUnder (TestTimeout)
      refresh() must eventuallyFailWith [IllegalStateException]
      failedStateIsReachedWithIllegalState
    }

  it must "fail refreshing a cluster when a known service " +
    "does not stabilize within grace period" in new ExpectRefresh("unregistered") {
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("INSTALLED")
      given(service3.state).willReturn("STARTED")
      refresh() must runUnder (TestTimeout)
      refresh() must eventuallyFailWith [IllegalStateException]
      failedStateIsReachedWithIllegalState
    }

  it must "refresh a cluster that stabilizes within grace period" in
    new ExpectRefresh("unregistered") {
      given(service1.state).willReturn("INSTALLED")
      given(service2.state).willReturn("INSTALLED").willReturn("STARTED")
      given(service3.state).willReturn("STARTED")
      refresh() must runUnder(TestTimeout)
      stateIsReached(Running)
    }

  it must "refresh a cluster in Terminated state if accessing its info returns 404 not found" in
    new ExpectRefresh("unregistered") {
      val notFound = failed(RequestException(mock[Request], "errorMessage", StatusCode(404)))
      given(cluster.getService(unknownService)).willReturn(notFound)
      refresh() must runUnder(TestTimeout)
      stateIsReached(Terminated)
    }

  trait BaseRefreshable extends Refreshing {
    override val infrastructureProvider = mock[InfrastructureProvider]
    override val provisioner = mock[ClusterProvisioner]
    override val refreshGracePeriod = 3 seconds
    val clientOnlyDescription = new AmbariServiceDescription with NoConfigurationContribution {
      val components: Seq[ComponentDescription] = Seq(
        ComponentDescription("", isMaster = true, isClient = true),
        ComponentDescription("", isMaster = false, isClient = true))
      val name: String = "ClientService"
    }
    val normalDescription = new AmbariServiceDescription with NoConfigurationContribution  {
      val components: Seq[ComponentDescription] = Seq(
        ComponentDescription("", isMaster = true, isClient = true),
        ComponentDescription("", isMaster = true, isClient = false))
      val name: String = "NormalService"
    }
    override val serviceDescriptions = Seq(clientOnlyDescription, normalDescription)
  }

  trait NoRefresh extends BaseRefreshable {
    def registerCluster(description: MutableClusterDescription) {
      fail("Should have not registered any cluster")
    }
  }

  abstract class ExpectRefresh(
      expectedRefreshedCluster: String,
      knownClusters: Seq[ClusterId] = Seq()
    ) extends BaseRefreshable {
    override val clusterIds = knownClusters
    val unknownService = "myUnknownService"
    val hostName = "myhost"
    val cluster = mock[Cluster]
    val service1 = mock[Service]
    val service2 = mock[Service]
    val service3 = mock[Service]
    val services = Seq(service1, service2, service3)
    val host = mock[Host]
    val cluster_machines_> = successful(Seq(mock[MachineState]))
    given(provisioner.listClusterNames).willReturn(successful(Seq(expectedRefreshedCluster)))
    given(provisioner.getCluster(expectedRefreshedCluster)).willReturn(successful(cluster))
    given(cluster.name).willReturn(expectedRefreshedCluster)
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

    @volatile
    private var registeredCluster: Option[MutableClusterDescription] = None

    def registerCluster(description: MutableClusterDescription) {
      description must (
        have ('id (ClusterId(expectedRefreshedCluster))) and
          have ('name (expectedRefreshedCluster)) and
          have ('size (1)) and
          have ('machines_> (cluster_machines_>))
        )
      description.deployment_>.onComplete(_ => registeredCluster = Some(description))
      description.nameNode_> must eventually (be (new URI(s"hdfs://$hostName:50070")))
    }

    def stateIsReached(state: ClusterState) =
      registeredCluster.get.whenInState(state) must runUnder(TestTimeout)

    def failedStateIsReachedWithIllegalState = {
      val description = registeredCluster.get
      val failedStateReached_> = description.whenInState(
        failedWithIllegalState andThen (_.matches),
        maxTransitions = 0
      )
      failedStateReached_> must runUnder(TestTimeout)
    }
  }

  def failedWithIllegalState: BeMatcher[ClusterState] = new BeMatcher[ClusterState] {
    def apply(state: ClusterState) = MatchResult(
      matches = state match {
        case Failed(e: IllegalStateException) => true
        case _ => false
      },
      failureMessage = s"expected Failed(IllegalStateException(...) but got $state",
      negatedFailureMessage = "unexpected failed(IllegalStateException(...) state"
    )
  }

}
