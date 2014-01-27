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

import scala.concurrent.duration._

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Second, Seconds, Span}

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.quota._
import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.clusters._

class CosmosMachineUsageDaoTest
  extends FlatSpec
  with MustMatchers
  with FutureMatchers
  with Eventually
  with OneInstancePerTest {

  override implicit def patienceConfig = PatienceConfig(
    timeout = Span(1, Second),
    interval = Span(10, Seconds)
  )

  val profileDao = new MockCosmosProfileDao
  val serviceManager = new MockedServiceManager(
    transitionDelay = 100.milliseconds,
    maxPoolSize = 16
  )
  val usageDao = new CosmosMachineUsageDao(profileDao, serviceManager)
  val profile = registerUser("myUser")(profileDao)

  "The usage dao" must "retrieve the machine pool size from the service manager" in {
    usageDao.machinePoolSize must be (16)
  }

  it must "get the used machines for all active clusters of a profile without filtering" in
    new WithUserClusters {
      usageDao.usageByProfile(requestedClusterId = None) must be (Map(
         profile.id -> 13
      ))
      serviceManager.terminateCluster(terminated) must eventuallySucceed
      usageDao.usageByProfile(requestedClusterId = None) must be (Map(
        profile.id -> 3
      ))
    }

  it must "get the used machines for all active clusters of a profile with filtering" in
    new WithUserClusters {
      val filtered = Option(clusterId2)
      usageDao.usageByProfile(filtered) must be (Map(
        profile.id -> 11
      ))
      serviceManager.terminateCluster(terminated)
      eventually {
        serviceManager.describeCluster(terminated).get.state must be (Terminated)
      }
      usageDao.usageByProfile(filtered) must be (Map(
        profile.id -> 1
      ))
    }

  it must "get the used machines considering the expected size of a cluster in failed state " +
    "that already had its machines provisioned" in
    new WithUserClusters {
      serviceManager.makeClusterFail(clusterId2, "Failure example") must eventuallySucceed
      usageDao.usageByProfile(requestedClusterId = None) must be (Map(
        profile.id -> 13
      ))
    }

  it must "get the used machines considering the expected size of a cluster in failed state " +
      "before provisioning any machines" in
      new WithUserClusters {
        import scalaz.Scalaz._

        val failedPreconditions = (clusterId: ClusterId) => () => "Failure example".failureNel
        val clusterId = serviceManager.createCluster(
          "failedCluster", 2, Seq.empty, Seq.empty, failedPreconditions)
        profileDao.withTransaction { implicit c =>
          profileDao.assignCluster(clusterId, profile.id)(c)
        }
        usageDao.usageByProfile(requestedClusterId = None) must be (Map(
          profile.id -> 13
        ))
      }

  it must "collect all groups and their member profiles" in new WithGroups {
    usageDao.globalGroupQuotas must be (GlobalGroupQuotas(Map(
      groupA -> Set(profile.id, profileA2.id),
      groupB -> Set(profileB1.id, profileB2.id)
    )))
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
    val groupA = GuaranteedGroup("A", Quota(3))
    val groupB = GuaranteedGroup("B", Quota(5))
    profileDao.withTransaction { implicit c =>
      profileDao.registerGroup(groupA)
      profileDao.registerGroup(groupB)
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
