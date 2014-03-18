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

package es.tid.cosmos.api.usage

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Second, Seconds, Span}

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.quota._
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.{ServiceManagerComponent, ClusterName}
import es.tid.cosmos.servicemanager.clusters._

class DynamicMachineUsageTest
  extends FlatSpec
  with MustMatchers
  with FutureMatchers
  with Eventually
  with OneInstancePerTest {

  override implicit def patienceConfig = PatienceConfig(
    timeout = Span(1, Second),
    interval = Span(10, Seconds)
  )

  "The usage dao" must "retrieve the machine pool size from the service manager" in
    new WithMachineUsage {
      machineUsage.machinePoolSize must be (16)
    }

  it must "get the used machines for all active clusters of a profile without filtering" in
    new WithUserClusters {
      machineUsage.usageByProfile(requestedClusterId = None) must be (Map(
         myUserProfile.id -> 13
      ))
      serviceManager.terminateCluster(terminated) must eventuallySucceed
      machineUsage.usageByProfile(requestedClusterId = None) must be (Map(
        myUserProfile.id -> 3
      ))
    }

  it must "get the used machines for all active clusters of a profile with filtering" in
    new WithUserClusters {
      val filtered = Option(clusterId2)
      machineUsage.usageByProfile(filtered) must be (Map(
        myUserProfile.id -> 11
      ))
      serviceManager.terminateCluster(terminated)
      eventually {
        serviceManager.describeCluster(terminated).get.state must be (Terminated)
      }
      machineUsage.usageByProfile(filtered) must be (Map(
        myUserProfile.id -> 1
      ))
    }

  it must "get the used machines considering the expected size of a cluster in failed state " +
    "that already had its machines provisioned" in
    new WithUserClusters {
      serviceManager.makeClusterFail(clusterId2, "Failure example") must eventuallySucceed
      machineUsage.usageByProfile(requestedClusterId = None) must be (Map(
        myUserProfile.id -> 13
      ))
    }

  it must "get the used machines considering the expected size of a cluster in failed state " +
      "before provisioning any machines" in
      new WithUserClusters {
        import scalaz.Scalaz._

        val failedPreconditions = (clusterId: ClusterId) => () => "Failure example".failureNel
        val clusterId = serviceManager.createCluster(
          ClusterName("failedCluster"), 2, Set.empty, Seq.empty, failedPreconditions)
        store.withTransaction { implicit c =>
          store.cluster.register(clusterId, myUserProfile.id)(c)
        }
        machineUsage.usageByProfile(requestedClusterId = None) must be (Map(
          myUserProfile.id -> 13
        ))
      }

  it must "collect all groups and their member profiles" in new WithGroups {
    machineUsage.globalGroupQuotas must be (GlobalGroupQuotas(Map(
      groupA -> Set(myUserProfile.id, profileA2.id),
      groupB -> Set(profileB1.id, profileB2.id)
    )))
  }

  private trait WithMachineUsage extends DynamicMachineUsageComponent
      with MockCosmosDataStoreComponent
      with ServiceManagerComponent {
    override val serviceManager = new MockedServiceManager(maxPoolSize = 16)
    val myUserProfile = registerUser("myUser")(store)
  }

  private trait WithUserClusters extends WithMachineUsage {
    val clusterId1 = serviceManager.createCluster(ClusterName("myCluster1"), 1, Set.empty, Seq.empty)
    val clusterId2 = serviceManager.createCluster(ClusterName("myCluster2"), 2, Set.empty, Seq.empty)
    val terminated = serviceManager.createCluster(
      ClusterName("terminatedCluster"), 10, Set.empty, Seq.empty)

    serviceManager.withCluster(clusterId1)(_.completeProvisioning())
    serviceManager.withCluster(clusterId2)(_.completeProvisioning())
    serviceManager.withCluster(terminated)(_.immediateTermination())

    store.withTransaction { c =>
      for (clusterId <- Seq(clusterId1, clusterId2, terminated)) {
        store.cluster.register(clusterId, myUserProfile.id)(c)
      }
    }
  }

  private trait WithGroups extends WithMachineUsage {
    val profileA2 = registerUser("userA2")(store)
    val profileB1 = registerUser("userB1")(store)
    val profileB2 = registerUser("userB2")(store)
    val clusterA1 = serviceManager.createCluster(ClusterName("clusterA1"), 1, Set.empty, Seq.empty)
    val clusterA2 = serviceManager.createCluster(ClusterName("clusterA2"), 2, Set.empty, Seq.empty)
    val clusterB1 = serviceManager.createCluster(ClusterName("clusterB1"), 1, Set.empty, Seq.empty)
    val clusterB2 = serviceManager.createCluster(ClusterName("clusterB2"), 2, Set.empty, Seq.empty)
    val terminated = serviceManager.createCluster(
      ClusterName("terminatedCluster"), 10, Set.empty, Seq.empty)
    val groupA = GuaranteedGroup("A", Quota(3))
    val groupB = GuaranteedGroup("B", Quota(5))
    store.withTransaction { implicit c =>
      store.group.register(groupA)
      store.group.register(groupB)
      store.profile.setGroup(myUserProfile.id, Some("A"))
      store.profile.setGroup(profileA2.id, Some("A"))
      store.profile.setGroup(profileB1.id, Some("B"))
      store.profile.setGroup(profileB2.id, Some("B"))
      store.cluster.register(clusterA1, myUserProfile.id)
      store.cluster.register(clusterA2, profileA2.id)
      store.cluster.register(clusterB1, profileB1.id)
      store.cluster.register(clusterB2, profileB2.id)
      store.cluster.register(terminated, profileB2.id)
    }
  }
}
