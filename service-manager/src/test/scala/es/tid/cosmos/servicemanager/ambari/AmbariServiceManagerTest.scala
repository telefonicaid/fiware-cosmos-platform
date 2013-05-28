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
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.concurrent.Future.successful
import scala.concurrent.Await
import scala.concurrent.duration._

import org.mockito.ArgumentMatcher
import org.mockito.BDDMockito.given
import org.mockito.Matchers.{any, eq => the, matches, argThat}
import org.mockito.Mockito.verify
import org.scalatest.OneInstancePerTest
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.platform.ial._
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.rest._
import es.tid.cosmos.servicemanager.ambari.services.{CosmosUserService, Hdfs, AmbariServiceDescription}
import es.tid.cosmos.servicemanager.ambari.configuration.Configuration
import es.tid.cosmos.servicemanager.ambari.ServiceMasterExtractor.ServiceMasterNotFound

class AmbariServiceManagerTest extends AmbariTestBase with OneInstancePerTest with MockitoSugar {

  val provisioner = mock[ClusterProvisioner]
  val infrastructureProvider = mock[InfrastructureProvider]
  val cluster = mock[Cluster]
  val serviceDescriptions = List(mock[AmbariServiceDescription], mock[AmbariServiceDescription])
  val services = List(mock[Service], mock[Service])
  val configurationContributions = List(contributionsWithNumber(1), contributionsWithNumber(2))
  val instance = new AmbariServiceManager(provisioner, infrastructureProvider, ClusterId("HDFS"))

  "A ServiceManager" must "have no Clusters by default" in {
    instance.clusterIds must be('empty)
  }

  it must "be able to create and terminate a single machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(1)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster("clusterName", 1, serviceDescriptions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts, clusterId)
  }

  it must "be able to create and terminate a multi-machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster("clusterName", 3, serviceDescriptions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts.tail, clusterId)
  }

  it must "be able to deploy the persistent HDFS cluster" in {
    val slaveCount = 10
    val (machines, hosts) = machinesAndHostsOf(slaveCount, "slaves")
    val (masterMachine, masterHost) = machinesAndHostsOf(1, "master")
    val hdfsService = mock[Service]
    val userService = mock[Service]
    def setExpectations() {
      given(infrastructureProvider.availableMachineCount(MachineProfile.HDFS_SLAVE))
        .willReturn(successful(slaveCount))
      setMachineExpectations(machines, hosts, MachineProfile.HDFS_SLAVE)
      setMachineExpectations(masterMachine, masterHost, MachineProfile.HDFS_MASTER)
      given(cluster.addHost(masterMachine.head.hostname)).willReturn(successful(masterHost.head))
      given(cluster.addHosts(machines.map(_.hostname))).willReturn(successful(hosts))
      given(provisioner.registeredHostnames).willReturn(
        successful((masterMachine ++ machines).map(_.hostname)))
      def mockAddComponents(hosts: Seq[Host]) {
        hosts.foreach(host => given(host.addComponents(any())).willReturn(successful()))
      }
      mockAddComponents(masterHost)
      mockAddComponents(hosts)
      def configureMockService(service: Service) {
        given(service.addComponent(any())).willReturn(successful(""))
        given(service.install()).willReturn(successful(service))
        given(service.start()).willReturn(successful(service))
      }
      configureMockService(hdfsService)
      configureMockService(userService)
      given(cluster.addService(Hdfs.name)).willReturn(successful(hdfsService))
      given(cluster.addService(CosmosUserService.name)).willReturn(successful(userService))
    }

    def verifyCalls() {
      verify(infrastructureProvider)
        .availableMachineCount(MachineProfile.HDFS_SLAVE)
      verify(infrastructureProvider)
        .createMachines(
            the(instance.persistentHdfsId.id),
            the(MachineProfile.HDFS_SLAVE),
            the(slaveCount),
            any())
      verify(infrastructureProvider)
        .createMachines(
            the(instance.persistentHdfsId.id),
            the(MachineProfile.HDFS_MASTER),
            the(1),
            any())
      val distinctHostnames = (masterMachine ++ machines).map(_.hostname).distinct
      verify(provisioner).bootstrapMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
      verify(provisioner).createCluster(instance.persistentHdfsId.id, "Cosmos-0.1.0")
      verify(cluster).addHost(masterMachine.head.hostname)
      verify(cluster).addHosts(machines.map(_.hostname))
      (hdfsService :: userService :: Nil).foreach(service => {
        verify(service).install()
        verify(service).start()
      })
      Hdfs.components.foreach(component => verify(hdfsService).addComponent(component.name))
      CosmosUserService.components.foreach(component =>
        verify(userService).addComponent(component.name))
    }

    setExpectations()
    get(instance.deployPersistentHdfsCluster())
    verifyCalls()
  }

  it must "fail adding users on an un-managed cluster" in {
    val unmanagedClusterId = ClusterId()
    evaluating {
      instance.addUsers(unmanagedClusterId, ClusterUser("username", "publicKey"))
    } must produce [IllegalArgumentException]
  }

  it must "add users on a cluster with Cosmos-user support" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    val services = serviceDescriptions :+ new CosmosUserService(Seq(ClusterUser("user1", "publicKey1")))
    val componentNames = CosmosUserService.components.map(_.name)
    setMachineExpectations(machines, hosts, services = services)
    setServiceExpectations()
    given(hosts.head.getComponentNames).willReturn(componentNames)
    val clusterId = instance.createCluster("clusterName", 3, services)
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    Await.ready(
      instance.addUsers(clusterId, ClusterUser("user2", "publicKey2")), Duration.Inf)
    Await.result(instance.terminateCluster(clusterId), 5 seconds)
    verifyClusterAndServices(machines, hosts.head, hosts.tail, clusterId)
    verify(cluster).addService(CosmosUserService.name)
    object ConfigurationMatcher extends ArgumentMatcher[Configuration] {
      def matches(argument: Any): Boolean = argument match {
        case conf: Configuration => {
          val propertyValues = conf.properties.values.toSeq
          propertyValues.contains("user2") && propertyValues.contains("publicKey2")
        }
      }
    }
    verify(cluster).applyConfiguration(argThat(ConfigurationMatcher), any())
  }

  it must "fail adding users on cluster without Cosmos-users support" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster("clusterName", 3, serviceDescriptions)
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    evaluating { Await.result(
      instance.addUsers(clusterId, ClusterUser("username", "publicKey")), Duration.Inf)
    } must produce [ServiceMasterNotFound]
  }

  it must "fail adding users on a cluster that is not in Running state" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster("clusterName", 3, serviceDescriptions)
    waitForClusterCompletion(clusterId, instance)
    terminateAndVerify(clusterId, instance)
    evaluating { Await.result(
        instance.addUsers(clusterId, ClusterUser("username", "publicKey")), Duration.Inf)
    } must produce [IllegalArgumentException]
  }

  def setMachineExpectations(
      machines: Seq[MachineState],
      hosts: Seq[Host],
      profile: MachineProfile.Value = MachineProfile.G1_COMPUTE,
      services: Seq[ServiceDescription] = Seq()) {
    given(infrastructureProvider.releaseMachines(any()))
      .willReturn(successful())
    given(infrastructureProvider.rootPrivateSshKey).willReturn("sshKey")
    given(provisioner.bootstrapMachines(any(), any())).willReturn(successful())
    given(provisioner.teardownMachines(any(), any())).willReturn(successful())
    given(infrastructureProvider.createMachines(any(), the(profile), any(), any()))
      .willReturn(Future.traverse(machines)(machine => successful(machine)))
    given(provisioner.createCluster(any(), any())).willReturn(successful(cluster))
    given(provisioner.removeCluster(any())).willReturn(successful())
    given(provisioner.getCluster(any())).willReturn(successful(cluster))
    given(provisioner.registeredHostnames).willReturn(successful(machines.map(_.hostname).toSeq))
    given(cluster.applyConfiguration(any(), any())).willReturn(successful())
    val serviceNames = services.map(_.name)
    given(cluster.serviceNames).willReturn(serviceNames)
    val serviceMock = mock[Service]
    given(serviceMock.addComponent(any())).willReturn(successful("componentName"))
    given(serviceMock.install()).willReturn(successful(serviceMock))
    given(serviceMock.start()).willReturn(successful(serviceMock))
    given(serviceMock.stop()).willReturn(successful(serviceMock))
    for (host <- hosts) {
      given(host.addComponents(any())).willReturn(successful())
    }
    given(cluster.addService(any())).willReturn(successful(serviceMock))
    given(cluster.getService(any())).willReturn(successful(serviceMock))
    given(cluster.getHosts).willReturn(successful(hosts))
    hosts.foreach(host => given(host.getComponentNames).willReturn(List()))
    given(cluster.addHost(machines.head.hostname)).willReturn(successful(hosts.head))
    given(cluster.addHosts(machines.tail.map(_.hostname))).willReturn(successful(hosts.tail))
  }

  def setServiceExpectations() {
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

  def tagPattern = matches("version\\d+")

  def verifyClusterAndServices(
    machines: Seq[MachineState],
    master: Host,
    slaves: Seq[Host],
    clusterId: ClusterId) {
    verify(infrastructureProvider).createMachines(
      the("clusterName"), the(MachineProfile.G1_COMPUTE), the(machines.size), any())
    verify(infrastructureProvider).releaseMachines(machines)
    verify(provisioner).createCluster(clusterId.toString, "Cosmos-0.1.0")
    val distinctHostnames = machines.map(_.hostname).distinct
    verify(provisioner).bootstrapMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
    verify(provisioner).removeCluster(clusterId.toString)
    verify(provisioner).teardownMachines(distinctHostnames, infrastructureProvider.rootPrivateSshKey)
    verify(cluster).applyConfiguration(
      the(mergedGlobalConfiguration(2, instance, master.name)), tagPattern)
    verify(cluster).applyConfiguration(the(mergedCoreConfiguration(2)), tagPattern)
    verify(cluster).applyConfiguration(the(contributionsWithNumber(1).services(0)), tagPattern)
    verify(cluster).applyConfiguration(the(contributionsWithNumber(2).services(0)), tagPattern)
    verify(cluster).addHost(machines.head.hostname)
    verify(cluster).addHosts(machines.tail.map(_.hostname))
    serviceDescriptions.foreach(sd => {
      verify(sd).createService(cluster, master, slaves)
      verify(sd).contributions(master.name)
    })
    services.foreach(service => {
      verify(service).install()
      verify(service).start()
    })
  }

  def machinesOf(numberOfMachines: Int, prefix: String): Seq[MachineState] =
    (1 to numberOfMachines).map(number =>
      MachineState(
        new Id(s"ID$number"), s"aMachineName$number",
        MachineProfile.G1_COMPUTE, MachineStatus.Running,
        s"hostname$prefix$number", s"ipAddress$number"))

  def hostsOf(numberOfHosts: Int): Seq[Host] = (1 to numberOfHosts).map(_ => mock[Host])

  def machinesAndHostsOf(numberOfInstances: Int, prefix: String = "") =
    (machinesOf(numberOfInstances, prefix), hostsOf(numberOfInstances))

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
