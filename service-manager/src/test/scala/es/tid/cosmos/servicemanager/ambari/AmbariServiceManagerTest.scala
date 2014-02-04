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
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.concurrent.Future.successful
import scala.concurrent.Await
import scala.concurrent.duration._
import scala.language.postfixOps
import scalaz.syntax.validation._

import org.mockito.ArgumentMatcher
import org.mockito.BDDMockito.given
import org.mockito.Matchers.{any, eq => the, matches, argThat}
import org.mockito.Mockito._
import org.scalatest.OneInstancePerTest
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.ExecutableValidation
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.ial._
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.ServiceMasterExtractor.ServiceMasterNotFound
import es.tid.cosmos.servicemanager.ambari.configuration.Configuration
import es.tid.cosmos.servicemanager.ambari.rest._
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.clusters._

class AmbariServiceManagerTest
  extends AmbariTestBase with OneInstancePerTest with MockitoSugar with FutureMatchers {

  val exclusiveMasterSizeCutoff = 10
  val provisioner = initializeProvisioner
  val infrastructureProvider = mock[InfrastructureProvider]
  given(infrastructureProvider.rootPrivateSshKey).willReturn("sshKey")
  val clusterManager = new AmbariClusterManager(provisioner, infrastructureProvider.rootPrivateSshKey)
  val cluster = mock[Cluster]
  val serviceDescriptions = List(mock[AmbariServiceDescription], mock[AmbariServiceDescription])
  val services = List(mock[Service], mock[Service])
  val configurationContributions = List(contributionsWithNumber(1), contributionsWithNumber(2))
  val instance = new AmbariServiceManager(
    clusterManager, infrastructureProvider,
    ClusterId("HDFS"), exclusiveMasterSizeCutoff, TestHadoopConfig,
    new AmbariClusterDao(
      new InMemoryClusterDao,
      initializeProvisioner,
      AmbariServiceManager.AllServices))
  val testTimeout = 1 second
  val NoPreconditions = UnfilteredPassThrough
  val stackVersion = "HDP-2.0.6_Cosmos"

  "A ServiceManager" must "have no Clusters by default" in {
    instance.clusterIds must be('empty)
  }

  it must "be able to create and terminate a single machine cluster" in {
    val (machines, hosts) = machinesAndHostsOf(1)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
      "clusterName", 1, serviceDescriptions, Seq(), NoPreconditions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    val clusterDescription = instance.describeCluster(clusterId).get
    clusterDescription.size must be (1)
    clusterDescription.nameNode must be (Some(new URI("hdfs://hostname1:50070")))
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts, clusterId)
  }

  it must "be able to track the users of a cluster upon creation" in {
    val (machines, hosts) = machinesAndHostsOf(1)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val users = Seq(ClusterUser(username = "jsmith", publicKey = "that public key"))
    val clusterId = instance.createCluster(
      "clusterName", 1, serviceDescriptions, users, NoPreconditions)
    waitForClusterCompletion(clusterId, instance)
    instance.listUsers(clusterId) must be (Some(users))
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts, clusterId)
  }

  it must "be able to create and terminate a multi-machine cluster" in {
    val ClusterSize = 5
    val (machines, hosts) = machinesAndHostsOf(ClusterSize)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
      "clusterName", ClusterSize, serviceDescriptions, Seq(), NoPreconditions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    val clusterDescription = instance.describeCluster(clusterId).get
    clusterDescription.size must be (ClusterSize)
    clusterDescription.nameNode must be (Some(new URI("hdfs://hostname1:50070")))
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(machines, hosts.head, hosts, clusterId)
  }

  it must "be able to create and terminate a multi-machine cluster with non-slave master node" in {
    val ClusterSize = 50
    val (machines, hosts) = machinesAndHostsOf(ClusterSize)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
      "clusterName", ClusterSize, serviceDescriptions, Seq(), NoPreconditions)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    val clusterDescription = instance.describeCluster(clusterId).get
    clusterDescription.size must be (ClusterSize)
    clusterDescription.nameNode must be (Some(new URI("hdfs://hostname1:50070")))
    terminateAndVerify(clusterId, instance)
    verifyClusterAndServices(
      machines, hosts.head, hosts.tail, clusterId, includeMasterAsSlave = false)
  }

  it must "be able to deploy the persistent HDFS cluster" in {
    val slaveCount = 10
    val (machines, hosts) = machinesAndHostsOf(slaveCount, "slaves")
    val (masterMachine, masterHost) = machinesAndHostsOf(1, "master")
    val hdfsService = mock[Service]
    val userService = mock[Service]
    def setExpectations() {
      given(infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave))
        .willReturn(successful(slaveCount))
      setMachineExpectations(machines, hosts, MachineProfile.HdfsSlave)
      setMachineExpectations(masterMachine, masterHost, MachineProfile.HdfsMaster)
      given(cluster.addHosts(machines.map(_.hostname))).willReturn(successful(hosts))
      given(provisioner.registeredHostnames).willReturn(
        successful((masterMachine ++ machines).map(_.hostname).toSet))
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
        .availableMachineCount(MachineProfile.HdfsSlave)
      verify(infrastructureProvider)
        .createMachines(
            any(),
            the(MachineProfile.HdfsSlave),
            the(slaveCount),
            any())
      verify(infrastructureProvider)
        .createMachines(
            any(),
            the(MachineProfile.HdfsMaster),
            the(1),
            any())
      val distinctHostnames = (masterMachine ++ machines).map(_.hostname).toSet
      verify(provisioner).bootstrapMachines(
        distinctHostnames,
        infrastructureProvider.rootPrivateSshKey)
      verify(provisioner).createCluster(instance.persistentHdfsId.id, stackVersion)
      verify(cluster).addHosts(any())
      Seq(hdfsService, userService).foreach(service => {
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
      instance.setUsers(unmanagedClusterId, Seq(ClusterUser("username", "publicKey")))
    } must produce [IllegalArgumentException]
  }

  it must "add users on a cluster with Cosmos-user support" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    val componentNames = CosmosUserService.components.map(_.name)
    setMachineExpectations(machines, hosts, services = serviceDescriptions)
    setServiceExpectations()
    given(hosts.head.getComponentNames).willReturn(componentNames)
    val clusterId = instance.createCluster(
      "clusterName",
      clusterSize = 3,
      serviceDescriptions,
      Seq(ClusterUser("user1", "publicKey1")),
      NoPreconditions)
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    get(instance.setUsers(clusterId, Seq(ClusterUser("user2", "publicKey2"))))
    Await.result(instance.terminateCluster(clusterId), 5 seconds)
    verifyClusterAndServices(machines, hosts.head, hosts, clusterId)
    verify(cluster).addService(CosmosUserService.name)
    object ConfigurationMatcher extends ArgumentMatcher[Configuration] {
      override def matches(argument: Any): Boolean = argument match {
        case conf: Configuration => {
          val propertyValues = conf.properties.values.toSeq
          propertyValues.contains("user2") && valueContains(propertyValues, "publicKey2")
            !propertyValues.contains("user1") && !valueContains(propertyValues, "publicKey1")
        }
      }

      private def valueContains(values: Seq[Any], token: String) = values.exists {
        case v: String => v.contains(token)
        case _ => false
      }
    }
    verify(cluster, atLeastOnce()).applyConfiguration(argThat(ConfigurationMatcher), any())
    val clusterUsers = instance.listUsers(clusterId)
    clusterUsers must be ('defined)
    clusterUsers.get must be (Seq(ClusterUser("user2", "publicKey2")))
  }

  it must "fail adding users on cluster without Cosmos-users support" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
      "clusterName", 3, serviceDescriptions, Seq(), NoPreconditions)
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    evaluating {
      get(instance.setUsers(clusterId, Seq(ClusterUser("username", "publicKey"))))
    } must produce [ServiceMasterNotFound]
  }

  it must "fail adding users on a cluster that is not in Running state" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
      "clusterName", 3, serviceDescriptions, Seq(), NoPreconditions)
    waitForClusterCompletion(clusterId, instance)
    terminateAndVerify(clusterId, instance)
    evaluating (get(instance.setUsers(clusterId, Seq(ClusterUser("username", "publicKey"))))) must
      produce [IllegalArgumentException]
  }

  it must "fail terminating a cluster that is not in a valid termination state" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
      "clusterName", 3, serviceDescriptions, Seq(), NoPreconditions)
    waitForClusterCompletion(clusterId, instance)
    terminateAndVerify(clusterId, instance)
    evaluating (instance.terminateCluster(clusterId)) must
      produce [IllegalArgumentException]
  }

  it must "pass through the preconditions to the infrastructure provider" in {
    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val willFailCondition: ClusterExecutableValidation = (_) => () => "Failed!".failureNel
    val clusterId = instance.createCluster(
      "clusterName", 3, serviceDescriptions, Seq(), willFailCondition)
    waitForClusterCompletion(clusterId, instance)
    verify(infrastructureProvider).createMachines(
      matchesValidation(willFailCondition(clusterId)), any(), any(), any())
  }

  it must "create cluster including service bundles" in {
    import ServiceDependencies._

    val (machines, hosts) = machinesAndHostsOf(3)
    setMachineExpectations(machines, hosts)
    setServiceExpectations()
    val clusterId = instance.createCluster(
          "clusterName", 3, Seq(Hive), Seq(), NoPreconditions)
    waitForClusterCompletion(clusterId, instance)
    val description = instance.describeCluster(clusterId)
    val expectedServices =
      (AmbariServiceManager.BasicHadoopServices ++ Seq(CosmosUserService, Hive)).withDependencies
    description.get.services must be (expectedServices.map(_.name).toSet)
  }

  private def initializeProvisioner = {
    val provisionerMock = mock[AmbariServer]
    given(provisionerMock.listClusterNames).willReturn(successful(Seq()))
    provisionerMock
  }

  private def setMachineExpectations(
      machines: Seq[MachineState],
      hosts: Seq[Host],
      profile: MachineProfile.Value = MachineProfile.G1Compute,
      services: Seq[ServiceDescription] = Seq()) {
    given(infrastructureProvider.releaseMachines(any()))
      .willReturn(successful())
    given(provisioner.bootstrapMachines(any(), any())).willReturn(successful())
    given(infrastructureProvider.createMachines(any(), the(profile), any(), any()))
      .willReturn(Future.traverse(machines)(machine => successful(machine)))
    given(infrastructureProvider.assignedMachines(any()))
      .willReturn(successful(machines))
    given(provisioner.createCluster(any(), any())).willReturn(successful(cluster))
    given(provisioner.removeCluster(any())).willReturn(successful())
    given(provisioner.getCluster(any())).willReturn(successful(cluster))
    given(provisioner.registeredHostnames).willReturn(successful(machines.map(_.hostname).toSet))
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
      given(host.getComponentNames).willReturn(Seq())
      given(cluster.getHost(host.name)).willReturn(successful(host))
    }
    given(cluster.addService(any())).willReturn(successful(serviceMock))
    given(cluster.getService(any())).willReturn(successful(serviceMock))
    given(cluster.getHosts).willReturn(successful(hosts.toSeq))
    given(cluster.addHosts(any())).willReturn(successful(hosts))
  }

  private def setServiceExpectations() {
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

  private def tagPattern = matches("version\\d+")

  private def verifyClusterAndServices(
    machines: Seq[MachineState],
    master: Host,
    slaves: Seq[Host],
    clusterId: ClusterId,
    includeMasterAsSlave: Boolean = true) {
    verify(infrastructureProvider).createMachines(
      any(), the(MachineProfile.G1Compute), the(machines.size), any())
    verify(infrastructureProvider).releaseMachines(machines)
    verify(provisioner).createCluster(clusterId.toString, stackVersion)
    val distinctHostnames = machines.map(_.hostname).toSet
    verify(provisioner).bootstrapMachines(
      distinctHostnames,
      infrastructureProvider.rootPrivateSshKey)
    verify(provisioner).removeCluster(clusterId.toString)
    val configTestHelper = new ConfiguratorTestHelpers(
      master.name, slaves.length, includeMasterAsSlave)
    verify(cluster).applyConfiguration(
      the(configTestHelper.mergedGlobalConfiguration(2, instance)), tagPattern)
    verify(cluster).applyConfiguration(the(configTestHelper.mergedCoreConfiguration(2)), tagPattern)
    verify(cluster).applyConfiguration(the(contributionsWithNumber(1).services(0)), tagPattern)
    verify(cluster).applyConfiguration(the(contributionsWithNumber(2).services(0)), tagPattern)
    verify(cluster).addHosts(any())
    serviceDescriptions.foreach(sd => {
      verify(sd).createService(cluster, master, slaves)
      verify(sd).contributions(configTestHelper.dynamicProperties)
    })
    services.foreach(service => {
      verify(service).install()
      verify(service).start()
    })
  }

  private def machinesOf(numberOfMachines: Int, prefix: String): Seq[MachineState] =
    (1 to numberOfMachines).map(number =>
      MachineState(
        new Id(s"ID$number"), s"aMachineName$number",
        MachineProfile.G1Compute, MachineStatus.Running,
        s"hostname$prefix$number", s"ipAddress$number"))

  private def hostsOf(numberOfHosts: Int, prefix: String): Seq[Host] = for {
    index <- 1 to numberOfHosts
  } yield {
    val host = mock[Host]
    given(host.name).willReturn(s"hostname$prefix$index")
    host
  }

  private def machinesAndHostsOf(numberOfInstances: Int, prefix: String = "") =
    (machinesOf(numberOfInstances, prefix), hostsOf(numberOfInstances, prefix))

  @tailrec
  private def waitForClusterCompletion(id: ClusterId, sm: ServiceManager): ClusterState = {
    val description = sm.describeCluster(id)
    description.get.state match {
      case Provisioning | Terminating => {
        Thread.sleep(500)
        waitForClusterCompletion(id, sm)
      }
      case Failed(reason) => throw new RuntimeException(reason)
      case _ => description.get.state
    }
  }

  private def terminateAndVerify(id: ClusterId, sm: ServiceManager) {
    sm.terminateCluster(id) must runUnder(testTimeout)
    val Some(terminatingDescription) = sm.describeCluster(id)
    terminatingDescription.state must (be (Terminated) or be (Terminating))
    waitForClusterCompletion(id, sm)
    sm.describeCluster(id).get.state must be (Terminated)
  }

  private def matchesValidation(expected: ExecutableValidation) =
    argThat(new ExecutableValidationMatcher(expected))

  private class ExecutableValidationMatcher(expected: ExecutableValidation)
      extends ArgumentMatcher[ExecutableValidation] {

    def matches(argument: Any): Boolean =
      expected() == argument.asInstanceOf[ExecutableValidation]()
  }
}
