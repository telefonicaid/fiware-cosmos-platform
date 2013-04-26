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

package es.tid.cosmos.servicemanager

import org.scalatest.{GivenWhenThen, OneInstancePerTest, FlatSpec}
import es.tid.cosmos.servicemanager.ambari._
import es.tid.cosmos.platform.manager.ial._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.util.Try
import es.tid.cosmos.platform.manager.ial.MachineState
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito.{given => givenMock}
import org.mockito.Matchers.{eq => isEq, any}
import org.mockito.Mockito.{times, verify, verifyNoMoreInteractions}
import es.tid.cosmos.platform.manager.ial.MachineState

/**
 * @author adamos
 */
class AmbariServiceManagerTest extends FlatSpec
with OneInstancePerTest with GivenWhenThen with MockitoSugar {

  val provisioner = mock[ProvisioningServer]
  val infrastructureProvider = mock[InfrastructureProvider]
  val cluster = mock[Cluster]
  val hdfs = mock[Service]
  val mapreduce = mock[Service]
  val instance = new AmbariServiceManager(provisioner, infrastructureProvider)

  "A ServiceManager" must "have no Clusters by default" in {
    Given("a manager with no clusters")
    Then("there should be no registed cluster IDs")
    assert(instance.clusterIds.isEmpty)
  }

  it should "be able to create a single machine cluster" in {
    Given("a medium sized machine in Running state")
    val machines = machinesOf(1)
    val hosts = hostsOf(1)
    setExpectations(machines, hosts)

    When("creating a cluster of size 1")
    val clusterId = instance.createCluster("clusterName", 1)
    val state = waitForClusterCompletion(clusterId, instance)

    Then("the cluster should consist of that single machine")
    And("the cluster should have all HDFS and MAPREDUCE services and components"
      + " on that single machine")
    verifyAllComponentsInHost(hosts.head)
    verifyClusterAndServices(machines)

    And("the cluster should have an assigned ID")
    assert(clusterId != null)

    And("the cluster should be running")
    assert(state === Running)
  }

  it should "be able to create a multi-machine cluster" in {
    Given("3 medium sized machines in Running state")
    val machines = machinesOf(3)
    val hosts = hostsOf(3)
    setExpectations(machines, hosts)

    When("creating a cluster of size 3")
    val clusterId = instance.createCluster("clusterName", 3)
    val state = waitForClusterCompletion(clusterId, instance)

    Then("the cluster should consist of those machines")
    And("it should have HDFS and MAPREDUCE services")
    And("it should have NAMENODE and HDFS_CLIENT components" +
      " on the 1st machine as Master")
    And("it should have DATANODE on the 2nd and 3rd machines as Slaves")
    verifyMasterAndSlaveComponents(master = hosts.head, slaves = hosts.tail)
    verifyClusterAndServices(machines)

    And("the cluster should have an assigned ID")
    assert(clusterId != null)
    And("the cluster should be running")
    assert(state === Running)
  }

  def setExpectations(machines: Seq[MachineState], hosts: Seq[Host]) {
    givenMock(infrastructureProvider.createMachines(any(), any(), any()))
      .willReturn(Try(machines.map(machine => fakeFuture(machine))))
    givenMock(provisioner.createCluster(any(), any())).willReturn(fakeFuture(cluster))
    givenMock(cluster.applyConfiguration(any(), any(), any())).willReturn(fakeFuture())
    machines.zip(hosts).foreach{case (machine, host) =>
      givenMock(cluster.addHost(machine.hostname)).willReturn(fakeFuture(host))}
    givenMock(cluster.addService("HDFS")).willReturn(fakeFuture(hdfs))
    givenMock(cluster.addService("MAPREDUCE")).willReturn(fakeFuture(mapreduce))
    givenMock(hdfs.install).willReturn(fakeFuture(hdfs))
    givenMock(mapreduce.install).willReturn(fakeFuture(mapreduce))
    givenMock(hdfs.start).willReturn(fakeFuture(hdfs))
    givenMock(mapreduce.start).willReturn(fakeFuture(mapreduce))
    givenMock(hdfs.addComponent(any())).willReturn(fakeFuture("A Service1 Component"))
    givenMock(mapreduce.addComponent(any())).willReturn(fakeFuture("A Service2 Component"))
    hosts.foreach(host => givenMock(host.addComponents(any())).willReturn(fakeFuture()))
  }

  def verifyAllComponentsInHost(host: Host) {
    verifyMasterAndSlaveComponents(master = host, slaves = Seq(host))
  }

  def verifyMasterAndSlaveComponents(master: Host, slaves: Seq[Host]) {
    verify(master).addComponents("NAMENODE", "HDFS_CLIENT")
    verify(master).addComponents("JOBTRACKER", "MAPREDUCE_CLIENT")
    for (slave <- slaves){
      verify(slave).addComponents("DATANODE")
      verify(slave).addComponents("TASKTRACKER")
    }
  }

  def verifyClusterAndServices(machines: Seq[MachineState]) {
    verify(infrastructureProvider).createMachines("clusterName", MachineProfile.M, machines.size)
    verify(provisioner).createCluster("clusterName", "HDP-1.2.0")
    verify(cluster).applyConfiguration(isEq("global"), isEq("version1"), any())
    verify(cluster).applyConfiguration(isEq("core-site"), isEq("version1"), any())
    verify(cluster).applyConfiguration(isEq("hdfs-site"), isEq("version1"), any())
    verify(cluster).applyConfiguration(isEq("mapred-site"), isEq("version1"), any())
    machines.foreach(m => verify(cluster).addHost(m.hostname))
    verify(cluster).addService("HDFS")
    verify(cluster).addService("MAPREDUCE")
    verify(hdfs).addComponent("NAMENODE")
    verify(hdfs).addComponent("DATANODE")
    verify(hdfs).addComponent("HDFS_CLIENT")
    verify(mapreduce).addComponent("JOBTRACKER")
    verify(mapreduce).addComponent("TASKTRACKER")
    verify(mapreduce).addComponent("MAPREDUCE_CLIENT")
    verify(hdfs).install
    verify(hdfs).start
    verify(mapreduce).install
    verify(mapreduce).start
    verifyNoMoreInteractions(infrastructureProvider, provisioner, cluster, hdfs, mapreduce)
  }

  def machinesOf(numberOfMachines: Int): Seq[MachineState] =
    (1 to numberOfMachines).map(number =>
      MachineState(
        new Id(s"ID$number"), s"aMachineName$number",
        MachineProfile.M, MachineStatus.Running,
        s"hostname$number", s"ipAddress$number"))

  def hostsOf(numberOfHosts: Int): Seq[Host] =
    (1 to numberOfHosts).map(_ => mock[Host])

  def fakeFuture[T](thing: T) = Future.successful(thing)

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
