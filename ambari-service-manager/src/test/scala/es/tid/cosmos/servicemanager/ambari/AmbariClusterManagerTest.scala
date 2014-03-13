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

import scala.concurrent.Future.successful

import org.mockito.BDDMockito.given
import org.mockito.Matchers.{eq => the, _}
import org.mockito.Mockito.{spy, verify}
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.common.scalatest.resources.TestResourcePaths
import es.tid.cosmos.servicemanager.{ClusterName, ComponentLocation, ComponentDescription}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.mocks.MockService
import es.tid.cosmos.servicemanager.ambari.rest._

class AmbariClusterManagerTest
  extends AmbariTestBase with MockitoSugar with FutureMatchers with TestResourcePaths {

  def tagPattern = matches("version\\d+")

  "The AmbariClusterManager" must "be able to deploy and remove a cluster" in new WithServer(10) {
    val deployment_> = instance.deployCluster(
      description,
      serviceDescriptions,
      configHelper.dynamicPropertiesFactory)

    deployment_> must eventuallySucceed
    verify(ambariServer).bootstrapMachines(
      hostDetails.map(_.hostname).toSet,
      "")
    val configTestHelper = new ConfiguratorTestHelpers(
      description.master.get.hostname, description.slaves.map(_.hostname))
    verify(cluster).applyConfiguration(
      the(configTestHelper.mergedGlobalConfiguration(2, instance)), tagPattern)
    verify(cluster).applyConfiguration(the(configTestHelper.mergedCoreConfiguration(2)), tagPattern)
    verify(cluster).applyConfiguration(the(contributionsWithNumber(1).services(0)), tagPattern)
    verify(cluster).applyConfiguration(the(contributionsWithNumber(2).services(0)), tagPattern)
    verify(cluster).addHosts(any())
    serviceDescriptions.foreach {sd =>
      verify(sd).createService(cluster, hosts.head, hosts)
      verify(sd).contributions(configTestHelper.dynamicProperties)
      val service = sd.serviceMock
      verify(service).install()
      verify(service).start()
    }

    val removeCluster_> = instance.removeCluster(description)
    removeCluster_> must eventuallySucceed
    verify(ambariServer).removeCluster(description.id.toString)
  }

  it must "be able to change the configuration of a service" in new WithServer(3) {
    given(ambariServer.getCluster(any())).willReturn(successful(cluster))
    val newConfig = contributionsWithNumber(3)
    val newServiceDescription = serviceDescriptions(0).copy(
      configuration = newConfig)
    val configurationChange_> = instance.changeServiceConfiguration(
      description, configHelper.dynamicPropertiesFactory, newServiceDescription)
    configurationChange_> must eventuallySucceed
    verify(cluster).applyConfiguration(the(newConfig.services(0)), tagPattern)
  }

  it must "fail to change the configuration of a service with no master" in new WithServer(3) {
    given(ambariServer.getCluster(any())).willReturn(successful(cluster))
    val newServiceDescription = serviceDescriptions(1).copy(
      configuration = contributionsWithNumber(3))
    val configurationChange_> = instance.changeServiceConfiguration(
      description, configHelper.dynamicPropertiesFactory, newServiceDescription)
    configurationChange_> must eventuallyFailWith[IllegalArgumentException]
  }

  class WithServer(hostCount: Int) {
    val ambariServer = mock[AmbariServer]
    given(ambariServer.bootstrapMachines(any(), any())).willReturn(successful())
    given(ambariServer.removeCluster(any())).willReturn(successful())
    given(ambariServer.registeredHostnames).willReturn(
      successful((1 to hostCount).map(_.toString).toSet))

    val cluster = mock[Cluster]
    given(ambariServer.createCluster(any(), any())).willReturn(successful(cluster))
    given(ambariServer.getCluster(any())).willReturn(successful(cluster))
    given(cluster.applyConfiguration(any(), any())).willReturn(successful())

    val serviceDescriptions = List(
      spy(MockService(
        "service1",
        Seq(
          ComponentDescription(
            "component1",
            distribution = Set(ComponentLocation.Master),
            isClient = true),
          ComponentDescription(
            "component2",
            distribution = Set(ComponentLocation.Slave),
            isClient = false)),
        contributionsWithNumber(1))),
      spy(MockService(
        "service2",
        Seq(ComponentDescription(
          "component3", distribution = Set(ComponentLocation.Slave), isClient = true)),
        contributionsWithNumber(2))))
    val serviceNames = serviceDescriptions.map(_.name)
    given(cluster.serviceNames).willReturn(serviceNames)
    for (sd <- serviceDescriptions) {
      val serviceMock = sd.serviceMock
      given(cluster.addService(sd.name)).willReturn(successful(serviceMock))
      given(cluster.getService(sd.name)).willReturn(successful(serviceMock))
    }

    val hosts = Seq.fill(hostCount)(mock[Host])
    val hostDetails = (1 to hostCount).map(idx => HostDetails(idx.toString, idx.toString))
    given(cluster.getHosts).willReturn(successful(hosts.toSeq))
    given(cluster.addHosts(any())).willReturn(successful(hosts))
    for ((host, details) <- hosts.zip(hostDetails)) {
      given(host.name).willReturn(details.hostname)
      given(host.addComponents(any())).willReturn(successful())
      given(cluster.getHost(host.name)).willReturn(successful(host))
    }
    given(hosts.head.getComponentNames).willReturn(Seq("component1"))
    hosts.tail.foreach { host =>
      given(host.getComponentNames).willReturn(Seq("component2", "component3"))
    }

    val description = ImmutableClusterDescription(
      ClusterId.random(),
      ClusterName("clusterName"),
      hostCount,
      Provisioning,
      nameNode = None,
      master = Some(hostDetails.head),
      slaves = hostDetails,
      users = None,
      services = Set())
    val configHelper = new ConfiguratorTestHelpers(
      description.master.get.hostname,
      description.slaves.map(_.hostname))
    val instance = new AmbariClusterManager(ambariServer, "", packageResourcesConfigDirectory)
  }
}
