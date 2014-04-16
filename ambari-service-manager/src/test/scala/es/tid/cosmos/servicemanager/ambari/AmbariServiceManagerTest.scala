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
import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future.successful
import scala.concurrent.duration._
import scala.language.postfixOps
import scalaz.syntax.validation._

import org.mockito.ArgumentMatcher
import org.mockito.BDDMockito.given
import org.mockito.Matchers.{eq => the, _}
import org.mockito.Mockito._
import org.scalatest.concurrent.Eventually
import org.scalatest.mock.MockitoSugar
import org.scalatest.time.{Millis, Seconds, Span}

import es.tid.cosmos.common.ExecutableValidation
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.ial._
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.clusters.InMemoryClusterDao
import es.tid.cosmos.servicemanager.ambari.mocks.MockAmbariClusterManager
import es.tid.cosmos.servicemanager.ambari.rest.AmbariTestBase
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services._
import es.tid.cosmos.servicemanager.services.InfinityDriver.InfinityDriverParameters
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

class AmbariServiceManagerTest
  extends AmbariTestBase with MockitoSugar with FutureMatchers with Eventually {

  override implicit def patienceConfig =
    PatienceConfig(timeout = Span(5, Seconds), interval = Span(100, Millis))

  private val testTimeout = 1 second

  private trait WithServiceManager {
    this: InfrastructureProviderComponent =>

    val exclusiveMasterSizeCutoff = 10
    val clusterManager = new MockAmbariClusterManager
    val serviceDescriptions: Set[AnyServiceInstance] = Set(
      Hive.instance,
      Pig.instance,
      InfinityDriver.instance(InfinityDriverParameters("secret")))
    lazy val instance = new AmbariServiceManager(
      clusterManager, infrastructureProvider,
      ClusterId("HDFS"), exclusiveMasterSizeCutoff, TestHadoopConfig,
      new InMemoryClusterDao)

    @tailrec
    final def waitForClusterCompletion(id: ClusterId, sm: ServiceManager): ClusterState = {
      val description = sm.describeCluster(id)
      description.get.state match {
        case Provisioning | Terminating =>
          Thread.sleep(500)
          waitForClusterCompletion(id, sm)
        case Failed(reason) => throw new RuntimeException(reason)
        case _ => description.get.state
      }
    }

    def terminateAndVerify(id: ClusterId, sm: ServiceManager): Unit = {
      sm.terminateCluster(id) must runUnder(testTimeout)
      val Some(terminatingDescription) = sm.describeCluster(id)
      terminatingDescription.state must (be (Terminated) or be (Terminating))
      waitForClusterCompletion(id, sm)
      sm.describeCluster(id).get.state must be (Terminated)
    }
  }

  private trait MockIalComponent extends InfrastructureProviderComponent {
    override val infrastructureProvider = mock[InfrastructureProvider]
    def machinesOf(numberOfMachines: Int, prefix: String = ""): Seq[MachineState] =
      (1 to numberOfMachines).map(number =>
        MachineState(
          new Id(s"ID$number"), s"aMachineName$number",
          MachineProfile.G1Compute, MachineStatus.Running,
          s"hostname$prefix$number", s"ipAddress$number"))
  }

  private abstract class WithMachines(numberOfMachines: Integer)
    extends MockIalComponent with WithServiceManager {

    def verifyCluster(
        machines: Seq[MachineState],
        clusterId: ClusterId,
        includeMasterAsSlave: Boolean = true): Unit = {
      verify(infrastructureProvider).createMachines(
        any(), the(MachineProfile.G1Compute), the(machines.size), any())
      verify(infrastructureProvider).releaseMachines(machines)
    }
    val machines = machinesOf(numberOfMachines)
    given(infrastructureProvider.releaseMachines(any()))
      .willReturn(successful())
    for (profile <- MachineProfile.values) {
      val machineFutures = Future.traverse(machines.filter(_.profile == profile))(successful)
      given(infrastructureProvider.createMachines(any(), the(profile), any(), any()))
        .willReturn(machineFutures)
    }
    given(infrastructureProvider.assignedMachines(any()))
      .willReturn(successful(machines))
  }

  "A ServiceManager" must "have no Clusters by default" in
    new WithServiceManager with MockIalComponent {
      instance.clusterIds must be('empty)
    }

  it must "be able to create and terminate a single machine cluster" in new WithMachines(1) {
    val clusterId = instance.createCluster(
      ClusterName("clusterName"), 1, serviceDescriptions, Seq(), UnfilteredPassThrough)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    val clusterDescription = instance.describeCluster(clusterId).get
    clusterDescription.size must be (1)
    clusterDescription.nameNode must be (Some(new URI("hdfs://hostname1:50070")))
    terminateAndVerify(clusterId, instance)
    verifyCluster(machines, clusterId)
  }

  it must "be able to track the users of a cluster upon creation" in new WithMachines(1) {
    val users = Seq(ClusterUser(username = "jsmith", publicKey = "that public key"))
    val clusterId = instance.createCluster(
      ClusterName("clusterName"), 1, serviceDescriptions, users, UnfilteredPassThrough)
    eventually {
      instance.listUsers(clusterId) must be (Some(users))
    }
    terminateAndVerify(clusterId, instance)
    verifyCluster(machines, clusterId)
  }

  it must "be able to create and terminate a multi-machine cluster" in new WithMachines(5) {
    val clusterId = instance.createCluster(
      ClusterName("clusterName"), machines.size, serviceDescriptions, Seq(), UnfilteredPassThrough)
    clusterId must not be null
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    val clusterDescription = instance.describeCluster(clusterId).get
    clusterDescription.size must be (machines.size)
    clusterDescription.nameNode must be (Some(new URI("hdfs://hostname1:50070")))
    terminateAndVerify(clusterId, instance)
    verifyCluster(machines, clusterId)
  }

  it must "be able to create and terminate a multi-machine cluster with non-slave master node" in
    new WithMachines(50) {
      val clusterId = instance.createCluster(
        ClusterName("clusterName"), machines.size, serviceDescriptions, Seq(), UnfilteredPassThrough)
      clusterId must not be null
      val state = waitForClusterCompletion(clusterId, instance)
      state must equal(Running)
      val clusterDescription = instance.describeCluster(clusterId).get
      clusterDescription.size must be (machines.size)
      clusterDescription.nameNode must be (Some(new URI("hdfs://hostname1:50070")))
      terminateAndVerify(clusterId, instance)
      verifyCluster(
        machines, clusterId, includeMasterAsSlave = false)
    }

  it must "be able to deploy the persistent HDFS cluster" in
    new WithServiceManager with MockIalComponent {
      val slaveCount = 10
      val machines = machinesOf(slaveCount, "slaves")
      val masterMachine = machinesOf(1, "master")
      given(infrastructureProvider.availableMachineCount(MachineProfile.HdfsSlave))
        .willReturn(successful(slaveCount))
      given(infrastructureProvider.releaseMachines(any()))
        .willReturn(successful())
      def setMachineExpectations(machines: Seq[MachineState], profile: MachineProfile.Value) = {
        val machineFutures = Future.traverse(machines)(successful)
        given(infrastructureProvider.createMachines(any(), the(profile), any(), any()))
          .willReturn(machineFutures)
      }
      given(infrastructureProvider.assignedMachines(any()))
        .willReturn(successful(machines))
      setMachineExpectations(machines, MachineProfile.HdfsSlave)
      setMachineExpectations(masterMachine, MachineProfile.HdfsMaster)

      def verifyCalls(): Unit = {
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
      }

      get(instance.deployPersistentHdfsCluster(InfinityServerParameters("http://api", "secret")))
      verifyCalls()
    }

  it must "fail adding users on an un-managed cluster" in
    new MockIalComponent with WithServiceManager {
      val unmanagedClusterId = ClusterId.random()
      evaluating {
        instance.setUsers(unmanagedClusterId, Seq(ClusterUser("username", "publicKey")))
      } must produce [IllegalArgumentException]
    }

  it must "add users on a cluster" in new WithMachines(3) {
    val clusterId = instance.createCluster(
      ClusterName("clusterName"),
      clusterSize = 3,
      serviceDescriptions,
      Seq(ClusterUser("user1", "publicKey1")),
      UnfilteredPassThrough)
    val state = waitForClusterCompletion(clusterId, instance)
    state must equal(Running)
    get(instance.setUsers(clusterId, Seq(ClusterUser("user2", "publicKey2"))))
    Await.result(instance.terminateCluster(clusterId), 5 seconds)
    verifyCluster(machines, clusterId)
    val clusterUsers = instance.listUsers(clusterId)
    clusterUsers must be ('defined)
    clusterUsers.get must be (Seq(ClusterUser("user2", "publicKey2")))
  }

  it must "fail adding users on a cluster that is not in Running state" in new WithMachines(3) {
    val clusterId = instance.createCluster(
      ClusterName("clusterName"), 3, serviceDescriptions, Seq(), UnfilteredPassThrough)
    waitForClusterCompletion(clusterId, instance)
    terminateAndVerify(clusterId, instance)
    evaluating (get(instance.setUsers(clusterId, Seq(ClusterUser("username", "publicKey"))))) must
      produce [IllegalArgumentException]
  }

  it must "fail terminating a cluster that is not in a valid termination state" in
    new WithMachines(3) {
      val clusterId = instance.createCluster(
        ClusterName("clusterName"), 3, serviceDescriptions, Seq(), UnfilteredPassThrough)
      waitForClusterCompletion(clusterId, instance)
      terminateAndVerify(clusterId, instance)
      evaluating (instance.terminateCluster(clusterId)) must
        produce [IllegalArgumentException]
    }

  it must "pass through the preconditions to the infrastructure provider" in new WithMachines(3) {
    val willFailCondition: ClusterExecutableValidation = (_) => () => "Failed!".failureNel
    val clusterId = instance.createCluster(
      ClusterName("clusterName"), 3, serviceDescriptions, Seq(), willFailCondition)
    waitForClusterCompletion(clusterId, instance)
    verify(infrastructureProvider).createMachines(
      matchesValidation(willFailCondition(clusterId)), any(), any(), any())
  }

  it must "create cluster including service bundles" in new WithMachines(3) {
    val clusterId = instance.createCluster(
      ClusterName("clusterName"),
      3,
      Set(Hive.instance, InfinityDriver.instance(InfinityDriverParameters("secret"))),
      Seq.empty,
      UnfilteredPassThrough
    )
    waitForClusterCompletion(clusterId, instance)
    val description = instance.describeCluster(clusterId)
    val expectedServices = new ServiceDependencyMapping(ServiceCatalogue)
      .resolve(AmbariServiceManager.BasicHadoopServices + Hive + CosmosUserService)
    description.get.services must be (expectedServices.map(_.name))
  }

  it must "be able to deploy the persistent hdfs" in new MockIalComponent with WithServiceManager {
    val nameNode = MachineState(
      new Id(s"NN"), s"TheNameNode",
      MachineProfile.HdfsMaster, MachineStatus.Running,
      s"hostname-namenode", s"ipAddress-namenode")
    val dataNode = MachineState(
      new Id(s"DN"), s"TheDataNode",
      MachineProfile.HdfsSlave, MachineStatus.Running,
      s"hostname-datanode", s"ipAddress-datanode")
    given(infrastructureProvider.releaseMachines(any())).willReturn(successful())
    given(infrastructureProvider.availableMachineCount(the(MachineProfile.HdfsSlave)))
      .willReturn(Future.successful(1))
    given(infrastructureProvider.createMachines(any(), the(MachineProfile.HdfsMaster), any(), any()))
      .willReturn(Future.successful(Seq(nameNode)))
    given(infrastructureProvider.createMachines(any(), the(MachineProfile.HdfsSlave), any(), any()))
      .willReturn(Future.successful(Seq(dataNode)))
    instance.describePersistentHdfsCluster() must be (None)
    val parameters = InfinityServerParameters("http://api-base/", "infinitySecret")
    val hdfsDeployment = instance.deployPersistentHdfsCluster(parameters)
    hdfsDeployment must eventuallySucceed
    val description = instance.describePersistentHdfsCluster().get
    val expectedServices = AmbariServiceManager.persistentHdfsServices(parameters)
    description.services must be (expectedServices.map(_.service.name).toSet)
  }

  it must "fail creation if a basic service that needs configuration is not configured" in new WithMachines(3) {
    evaluating { instance.createCluster(
      ClusterName("clusterName"),
      3,
      Set(Hive.instance),
      Seq.empty,
      UnfilteredPassThrough
    ) } must produce [IllegalArgumentException]
  }

  private def matchesValidation(expected: ExecutableValidation) =
    argThat(new ExecutableValidationMatcher(expected))

  private class ExecutableValidationMatcher(expected: ExecutableValidation)
      extends ArgumentMatcher[ExecutableValidation] {

    def matches(argument: Any): Boolean =
      expected() == argument.asInstanceOf[ExecutableValidation]()
  }
}
