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

package es.tid.cosmos.api.profile

import scala.Some
import scala.concurrent.duration._

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers

class CosmosMachineUsageDaoTest
  extends FlatSpec
  with MustMatchers
  with FutureMatchers
  with OneInstancePerTest {

  val profileDao = new MockCosmosProfileDao
  val serviceManager = new MockedServiceManager(
    transitionDelay = 100.milliseconds,
    maxPoolSize = 15
  )
  val usageDao = new CosmosMachineUsageDao(profileDao, serviceManager)
  val profile = registerUser("myUser")(profileDao)

  "The usage dao" must "retrieve the machine pool size from the service manager" in {
    usageDao.machinePoolSize must be (15)
  }

  it must "get the used machines for all active clusters of a profile without filtering" in
    new WithUserClusters {
      usageDao.withoutClusterFilter.usedMachinesForActiveClusters(profile) must be (13)
      serviceManager.terminateCluster(terminated) must eventuallySucceed
      usageDao.withoutClusterFilter.usedMachinesForActiveClusters(profile) must be (3)
    }

  it must "get the used machines for all active clusters of a profile with filtering" in
    new WithUserClusters {
      val filtered = Option(clusterId2)
      usageDao.withClusterFilter(filtered).usedMachinesForActiveClusters(profile) must be (11)
      serviceManager.terminateCluster(terminated) must eventuallySucceed
      usageDao.withClusterFilter(filtered).usedMachinesForActiveClusters(profile) must be (1)
    }

  it must "get no groups and no used machines by default" in {
    usageDao.withoutClusterFilter.usedMachinesByGroups must be (Map(NoGroup -> 0))
  }

  it must "get the used machines for all existing groups without filtering" in new WithGroups {
    usageDao.withoutClusterFilter.usedMachinesByGroups must be (Map(
      NoGroup -> 0,
      GuaranteedGroup("A", FiniteQuota(3)) -> 3,
      GuaranteedGroup("B", FiniteQuota(5)) -> 13
    ))
    serviceManager.terminateCluster(terminated) must eventuallySucceed
    usageDao.withoutClusterFilter.usedMachinesByGroups must be (Map(
      NoGroup -> 0,
      GuaranteedGroup("A", FiniteQuota(3)) -> 3,
      GuaranteedGroup("B", FiniteQuota(5)) -> 3
    ))
  }

  it must "get the used machines for all existing groups with filtering" in new WithGroups {
    usageDao.withClusterFilter(Some(clusterA2)).usedMachinesByGroups must be (Map(
      NoGroup -> 0,
      GuaranteedGroup("A", FiniteQuota(3)) -> 1,
      GuaranteedGroup("B", FiniteQuota(5)) -> 13
    ))
    serviceManager.terminateCluster(terminated) must eventuallySucceed
    usageDao.withClusterFilter(Some(clusterA2)).usedMachinesByGroups must be (Map(
      NoGroup -> 0,
      GuaranteedGroup("A", FiniteQuota(3)) -> 1,
      GuaranteedGroup("B", FiniteQuota(5)) -> 3
    ))
  }

  "A user without clusters" must "have 0 used machines" in {
    usageDao.withoutClusterFilter.usedMachinesForActiveClusters(profile) must be (0)
    usageDao.withClusterFilter(Some(ClusterId())).usedMachinesForActiveClusters(profile) must be (0)
  }

  private trait WithUserClusters {
    val clusterId1 = serviceManager.createCluster("myCluster1", 1, Seq.empty, Seq.empty)
    val clusterId2 = serviceManager.createCluster("myCluster2", 2, Seq.empty, Seq.empty)
    val terminated = serviceManager.createCluster("terminatedCluster", 10, Seq.empty, Seq.empty)
    profileDao.withTransaction { c =>
      for (clusterId <- Seq(clusterId1, clusterId2, terminated))
        profileDao.assignCluster(clusterId, profile.id)(c)
    }
  }
  
  private trait WithGroups {
    val profileA2 = registerUser("userA2")(profileDao)
    val profileB1 = registerUser("userB1")(profileDao)
    val profileB2 = registerUser("userB2")(profileDao)
    val clusterA1 = serviceManager.createCluster("clusterA1", 1, Seq.empty, Seq.empty)
    val clusterA2 = serviceManager.createCluster("clusterA2", 2, Seq.empty, Seq.empty)
    val clusterB1 = serviceManager.createCluster("clusterB1", 1, Seq.empty, Seq.empty)
    val clusterB2 = serviceManager.createCluster("clusterB2", 2, Seq.empty, Seq.empty)
    val terminated = serviceManager.createCluster("terminatedCluster", 10, Seq.empty, Seq.empty)
    profileDao.withTransaction { implicit c =>
      profileDao.registerGroup(GuaranteedGroup("A", Quota(3)))
      profileDao.registerGroup(GuaranteedGroup("B", Quota(5)))
      profileDao.setGroup(profile.id, Some("A"))
      profileDao.setGroup(profileA2.id, Some("A"))
      profileDao.setGroup(profileB1.id, Some("B"))
      profileDao.setGroup(profileB2.id, Some("B"))
      profileDao.assignCluster(clusterA1, profile.id)
      profileDao.assignCluster(clusterA2, profileA2.id)
      profileDao.assignCluster(clusterB1, profileB1.id)
      profileDao.assignCluster(clusterB2, profileB2.id)
      profileDao.assignCluster(terminated, profileB2.id)
    }
  }
}
