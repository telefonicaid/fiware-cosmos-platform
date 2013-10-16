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
import scala.concurrent.promise
import scala.concurrent.Future._
import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits.global

import com.ning.http.client.Request
import dispatch.StatusCode
import org.mockito.BDDMockito._
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.ial.{InfrastructureProvider, MachineState}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.rest._
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.ambari.configuration.NoConfigurationContribution

class RefreshingTest extends AmbariTestBase with MockitoSugar with FutureMatchers {

  val HostName = "myhost"
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
      given(clientOnlyService.state).willReturn("INSTALLED")
      given(normalService.state).willReturn("STARTED")
      given(unknownService.state).willReturn("STARTED")
      refresh() must runUnder(TestTimeout)
      stateIsReached(Running)
    }

  it must "register only any unregistered clusters the provisioner yields upon a refresh" in
    new ExpectRefresh("unregistered", knownClusters = Seq(ClusterId("registered"))) {
      given(clientOnlyService.state).willReturn("INSTALLED")
      given(normalService.state).willReturn("STARTED")
      given(unknownService.state).willReturn("STARTED")
      refresh() must runUnder(TestTimeout)
      stateIsReached(Running)
    }

  it must "refresh a cluster to Failed state when a unknown service does not stabilize within " +
    "grace period" in new ExpectRefresh("unregistered") {
      given(clientOnlyService.state).willReturn("INSTALLED")
      given(normalService.state).willReturn("STARTED")
      given(unknownService.state).willReturn("INSTALLED")
      refresh() must (runUnder (TestTimeout) and eventually (equal (())))
      failedStateIsReachedWithIllegalState()
    }

  it must "refresh a cluster to Failed state when a known service " +
    "does not stabilize within grace period" in new ExpectRefresh("unregistered") {
      given(clientOnlyService.state).willReturn("INSTALLED")
      given(normalService.state).willReturn("INSTALLED")
      given(unknownService.state).willReturn("STARTED")
      refresh() must (runUnder (TestTimeout) and eventually (equal (())))
      failedStateIsReachedWithIllegalState()
    }

  it must "refresh a cluster that stabilizes within grace period" in
    new ExpectRefresh("unregistered") {
      given(clientOnlyService.state).willReturn("INSTALLED")
      given(normalService.state).willReturn("INSTALLED").willReturn("STARTED")
      given(unknownService.state).willReturn("INSTALLED").willReturn("INSTALLED").willReturn("STARTED")
      refresh() must runUnder(TestTimeout)
      stateIsReached(Running)
    }

  it must "refresh a cluster in Terminated state if accessing its info returns 404 not found" in
    new ExpectRefresh("unregistered") {
      whenGetServiceReturns404()
      refresh() must runUnder(TestTimeout)
      stateIsReached(Terminated)
    }

  abstract class BaseRefreshable extends Refreshing {
    override val infrastructureProvider = mockInfrastructureProvider()
    override val provisioner = mock[ClusterProvisioner]
    override val refreshGracePeriod = 3 seconds
    val clientOnlyDescription = new AmbariServiceDescription with NoConfigurationContribution {
      val components: Seq[ComponentDescription] = Seq(
        ComponentDescription("", isMaster = true, isClient = true),
        ComponentDescription("", isMaster = false, isClient = true))
      val name: String = "ClientService"
    }
    val clientOnlyService = mockService(clientOnlyDescription.name)
    val normalDescription = new AmbariServiceDescription with NoConfigurationContribution  {
      val components: Seq[ComponentDescription] = Seq(
        ComponentDescription("", isMaster = true, isClient = true),
        ComponentDescription("", isMaster = true, isClient = false))
      val name: String = "NormalService"
    }
    val normalService = mockService(normalDescription.name)
    val unknownServiceName = "myUnknownService"
    val unknownService = mockService(unknownServiceName)
    override val allServices = Seq(clientOnlyDescription, normalDescription)

    protected def registerClusterInProvisioner(clusterName: String, cluster: Cluster) {
      given(provisioner.listClusterNames).willReturn(successful(Seq(clusterName)))
      given(provisioner.getCluster(clusterName)).willReturn(successful(cluster))
    }

    private def mockService(name: String) = {
      val service = mock[Service]
      given(service.name).willReturn(name)
      service
    }

    private def mockInfrastructureProvider() = {
      val provider = mock[InfrastructureProvider]
      val cluster_machines_> = successful(Seq(mock[MachineState]))
      given(provider.assignedMachines(Seq(HostName))).willReturn(cluster_machines_>)
      provider
    }
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
    private val cluster = mockCluster(Seq(clientOnlyService, normalService, unknownService))
    registerClusterInProvisioner(expectedRefreshedCluster, cluster)

    private val registeredCluster = promise[MutableClusterDescription]()

    def registerCluster(description: MutableClusterDescription) {
      description must (
        have ('id (ClusterId(expectedRefreshedCluster))) and
          have ('name (expectedRefreshedCluster)) and
          have ('size (1)) and
          have ('machines_> (infrastructureProvider.assignedMachines(Seq(HostName))))
        )
      registeredCluster.success(description)
      description.nameNode_> must eventually (be (new URI(s"hdfs://$HostName:50070")))
    }

    def waitForCondition(
        description: MutableClusterDescription,
        condition: ClusterState => Boolean,
        remainingTime: FiniteDuration = TestTimeout): Boolean = {
      val sleepTime = 1 second

      if (remainingTime <= (0 seconds)) false
      else if (condition(description.state)) true
      else {
        Thread.sleep(sleepTime.toMillis)
        waitForCondition(description, condition, remainingTime - sleepTime)
      }
  }

    def stateIsReached(targetState: ClusterState) = conditionIsReached(_ == targetState)

    def conditionIsReached(condition: ClusterState => Boolean) {
      registeredCluster.future.map(waitForCondition(_, condition)) must eventually(equal (true))
    }

    def failedStateIsReachedWithIllegalState() = conditionIsReached({
      case Failed(e: IllegalStateException) => true
      case _ => false
    })

    def whenGetServiceReturns404() {
      val notFound = failed(RequestException(mock[Request], "errorMessage", StatusCode(404)))
      given(cluster.getService(unknownServiceName)).willReturn(notFound)
    }

    private def mockHost(hostName: String) = {
      val host = mock[Host]
      given(host.getComponentNames).willReturn(Seq("NAMENODE"))
      given(host.name).willReturn(hostName)
      host
    }

    private def mockCluster(services: Seq[Service]) = {
      val cluster = mock[Cluster]
      given(cluster.name).willReturn(expectedRefreshedCluster)
      given(cluster.hostNames).willReturn(Seq(HostName))
      val host = mockHost(HostName)
      given(cluster.getHosts).willReturn(successful(Seq(host)))
      val serviceNames = services.map(_.name)
      given(cluster.serviceNames).willReturn(serviceNames)
      services.foreach(srv => given(cluster.getService(srv.name)).willReturn(successful(srv)))
      cluster
    }
  }
}
