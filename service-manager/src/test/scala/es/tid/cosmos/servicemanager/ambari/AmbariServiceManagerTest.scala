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

import scala.util.Try

import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.mockito.Mockito.{verify, verifyNoMoreInteractions}
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.platform.manager.ial._
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.FutureTestHelpers.fakeFuture
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, ClusterProvisioner, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription

/**
 * @author adamos
 */
class AmbariServiceManagerTest extends FlatSpec
with OneInstancePerTest with MustMatchers with MockitoSugar {

  val provisioner = mock[ClusterProvisioner]
  val infrastructureProvider = mock[InfrastructureProvider]
  val cluster = mock[Cluster]
  val serviceDescriptions = List(mock[AmbariServiceDescription], mock[AmbariServiceDescription])
  val services = List(mock[Service], mock[Service])
  val configurationContributions = List(contributionsWithNumber(1), contributionsWithNumber(2))
  val instance = new AmbariServiceManager(provisioner, infrastructureProvider)

  "A ServiceManager" must "have no Clusters by default" in {
    instance.clusterIds must be('empty)
  }

  it should "be able to create a single machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(1)
    setExpectations(machines, hosts)
    val clusterId = instance.createCluster("clusterName", 1, serviceDescriptions)
    val state = waitForClusterCompletion(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts)
    clusterId must not be null
    state must equal(Running)
  }

  it should "be able to create a multi-machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setExpectations(machines, hosts)
    val clusterId = instance.createCluster("clusterName", 3, serviceDescriptions)
    val state = waitForClusterCompletion(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts.tail)
    clusterId must not be null
    state must equal(Running)
  }

  def setExpectations(machines: Seq[MachineState], hosts: Seq[Host]) {
    given(infrastructureProvider.createMachines(any(), any(), any()))
      .willReturn(Try(machines.map(machine => fakeFuture(machine))))
    given(provisioner.createCluster(any(), any())).willReturn(fakeFuture(cluster))
    given(cluster.applyConfiguration(any())).willReturn(fakeFuture())
    machines.zip(hosts).foreach {
      case (machine, host) =>
        given(cluster.addHost(machine.hostname)).willReturn(fakeFuture(host))
    }
    serviceDescriptions.zip(services).foreach({
      case (description, service) =>
        given(description.createService(any(), any(), any()))
          .willReturn(fakeFuture(service))
    })
    serviceDescriptions.zip(configurationContributions).foreach({
      case (description, contribution) =>
        given(description.contributions(any())).willReturn(contribution)
    })
    services.foreach(service => {
      given(service.install()).willReturn(fakeFuture(service))
      given(service.start()).willReturn(fakeFuture(service))
    })
  }

  def verifyClusterAndServices(machines: Seq[MachineState],
                               master: Host,
                               slaves: Seq[Host]) {
    verify(infrastructureProvider)
      .createMachines("clusterName", MachineProfile.M, machines.size)
    verify(provisioner).createCluster("clusterName", "HDP-1.2.0")
    verify(cluster).applyConfiguration(mergedGlobalConfiguration(2, instance, master.name))
    verify(cluster).applyConfiguration(mergedCoreConfiguration(2))
    verify(cluster).applyConfiguration(contributionsWithNumber(1).services(0))
    verify(cluster).applyConfiguration(contributionsWithNumber(2).services(0))
    machines.foreach(m => verify(cluster).addHost(m.hostname))
    serviceDescriptions.foreach(sd => {
      verify(sd).createService(cluster, master, slaves)
      verify(sd).contributions(master.name)
    })
    services.foreach(service => {
      verify(service).install()
      verify(service).start()
    })
    verifyNoMoreInteractions(infrastructureProvider, provisioner, cluster,
      services(0), services(1), serviceDescriptions(0), serviceDescriptions(1))
  }

  def machinesOf(numberOfMachines: Int): Seq[MachineState] =
    (1 to numberOfMachines).map(number =>
      MachineState(
        new Id(s"ID$number"), s"aMachineName$number",
        MachineProfile.M, MachineStatus.Running,
        s"hostname$number", s"ipAddress$number"))

  def hostsOf(numberOfHosts: Int): Seq[Host] = (1 to numberOfHosts).map(_ => mock[Host])

  def machinesAndHostsOf(numberOfInstances: Int)=
    (machinesOf(numberOfInstances), hostsOf(numberOfInstances))

  def waitForClusterCompletion(id: ClusterId, sm: ServiceManager):
  ClusterState = {
    val description = sm.describeCluster(id)
    description.get.state match {
      case Provisioning => {
        Thread.sleep(1000)
        waitForClusterCompletion(id, sm)
      }
      case Failed(reason) => throw reason
      case _ => description.get.state
    }
  }
}
