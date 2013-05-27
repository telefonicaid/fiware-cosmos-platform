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

import scala.annotation.tailrec
import scala.concurrent.Future.successful
import scala.util.Try

import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.mockito.Mockito.verify
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.platform.ial._
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, ClusterProvisioner, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.Failed

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

  it must "be able to create and terminate a single machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(1)
    setExpectations(machines, hosts)
    val clusterId = instance.createCluster("clusterName", 1, serviceDescriptions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts, clusterId)
  }

  it must "be able to create and terminate a multi-machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setExpectations(machines, hosts)
    val clusterId = instance.createCluster("clusterName", 3, serviceDescriptions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts.tail, clusterId)
  }

  def setExpectations(machines: Seq[MachineState], hosts: Seq[Host]) {
    given(infrastructureProvider.createMachines(any(), any(), any()))
      .willReturn(Try(machines.map(machine => successful(machine))))
    given(infrastructureProvider.releaseMachines(any()))
      .willReturn(successful())
    given(infrastructureProvider.rootSshKey).willReturn("sshKey")
    given(provisioner.bootstrapMachines(any(), any())).willReturn(successful())
    given(provisioner.teardownMachines(any(), any())).willReturn(successful())
    given(provisioner.createCluster(any(), any())).willReturn(successful(cluster))
    given(provisioner.removeCluster(any())).willReturn(successful())
    given(provisioner.getCluster(any())).willReturn(successful(cluster))
    given(provisioner.registeredHostnames).willReturn(successful(machines.map(_.hostname).toSeq))
    given(cluster.applyConfiguration(any())).willReturn(successful())
    given(cluster.serviceNames).willReturn(List())
    machines.zip(hosts).foreach {
      case (machine, host) =>
        given(cluster.addHost(machine.hostname)).willReturn(successful(host))
    }
    serviceDescriptions.zip(services).foreach({
      case (description, service) =>
        given(description.createService(any(), any(), any()))
          .willReturn(successful(service))
    })
    serviceDescriptions.zip(configurationContributions).foreach({
      case (description, contribution) =>
        given(description.contributions(any())).willReturn(contribution)
    })
    services.foreach(service => {
      given(service.install()).willReturn(successful(service))
      given(service.start()).willReturn(successful(service))
    })
  }

  def verifyClusterAndServices(
    machines: Seq[MachineState],
    master: Host,
    slaves: Seq[Host],
    clusterId: ClusterId) {
    verify(infrastructureProvider)
      .createMachines("clusterName", MachineProfile.M, machines.size)
    verify(infrastructureProvider).releaseMachines(machines)
    verify(provisioner).createCluster(clusterId.toString, "Cosmos-0.1.0")
    verify(provisioner).bootstrapMachines(machines, infrastructureProvider.rootSshKey)
    verify(provisioner).removeCluster(clusterId.toString)
    verify(provisioner).teardownMachines(machines, infrastructureProvider.rootSshKey)
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

  @tailrec
  private def waitForClusterCompletion(id: ClusterId, sm: ServiceManager): ClusterState = {
    val description = sm.describeCluster(id)
    description.get.state match {
      case Provisioning | Terminating => {
        Thread.sleep(500)
        waitForClusterCompletion(id, sm)
      }
      case Failed(reason) => throw reason
      case _ => description.get.state
    }
  }

  def terminateAndVerify(id: ClusterId, sm: ServiceManager) {
    sm.terminateCluster(id)
    val Some(terminatingDescription) = sm.describeCluster(id)
    terminatingDescription.state must (be (Terminated) or be (Terminating))
    waitForClusterCompletion(id, sm)
    sm.describeCluster(id).get.state must be (Terminated)
  }
}
