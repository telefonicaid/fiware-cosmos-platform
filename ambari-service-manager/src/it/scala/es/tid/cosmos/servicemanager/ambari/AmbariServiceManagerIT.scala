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
import scala.concurrent.Await
import scala.concurrent.duration._
import scala.language.postfixOps

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.resources.TestResourcePaths
import es.tid.cosmos.common.scalatest.tags.HasExternalDependencies
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.clusters.InMemoryClusterDao
import es.tid.cosmos.servicemanager.ambari.configuration.HadoopConfig
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescriptionFactory._
import es.tid.cosmos.servicemanager.clusters._

class AmbariServiceManagerIT extends FlatSpec with MustMatchers with BeforeAndAfter
  with FakeInfrastructureProviderComponent
  with TestResourcePaths {

  val preConditions = UnfilteredPassThrough

  val hadoopConfig = HadoopConfig(
    mrAppMasterMemory = 100,
    mapTaskMemory = 200,
    mapHeapMemory = 100,
    mappersPerSlave = 8,
    reduceTaskMemory = 200,
    reduceHeapMemory = 100,
    reducersPerSlave = 4,
    yarnTotalMemory = 1024,
    yarnContainerMinimumMemory = 100,
    yarnVirtualToPhysicalMemoryRatio = 2.1,
    nameNodeHttpPort = 50070,
    zookeeperPort = 1234,
    servicesConfigDirectory = resourcesConfigDirectory
  )

  @tailrec
  final def waitForClusterCompletion(id: ClusterId, sm: ServiceManager): ClusterState = {
    val description = sm.describeCluster(id)
    description.get.state match {
      case Provisioning | Terminating =>
        Thread.sleep(1000)
        waitForClusterCompletion(id, sm)
      case _ => description.get.state
    }
  }

  var sm: AmbariServiceManager = null

  before {
    val ambariServer = new AmbariServer("10.95.161.137", 8080, "admin", "admin")
    val clusterManager = new AmbariClusterManager(
      ambariServer, infrastructureProvider.rootPrivateSshKey, hadoopConfig.servicesConfigDirectory)
    sm = new AmbariServiceManager(
      clusterManager, infrastructureProvider,
      ClusterId("hdfs"), exclusiveMasterSizeCutoff = 10,
      hadoopConfig,
      new AmbariClusterDao(
        new InMemoryClusterDao,
        ambariServer,
        AmbariServiceManager.AllServices.map(
          toAmbariService(_, hadoopConfig.servicesConfigDirectory))
      )
    )
  }

  "Ambari server" must "create and terminate cluster" taggedAs HasExternalDependencies in {
    val user1 = ClusterUser("luckydude1", "publicKey1")
    val user2 = ClusterUser("luckydude2", "publicKey2")
    val user3 = ClusterUser("luckydude3", "publicKey2")

    val id = sm.createCluster(
      name = ClusterName("persistentHdfsId"),
      clusterSize = 1,
      serviceInstances = Set.empty,
      users = Seq(user1, user2),
      preConditions = preConditions
    )
    println("Cluster creating...")
    val description = sm.describeCluster(id).get
    description.state must be (Provisioning)
    println("Starting wait for cluster completion...")
    val endState = waitForClusterCompletion(id, sm)
    println("Cluster completed.")
    endState must be === Running
    val updatedService = sm.setUsers(id, Seq(user1, user3))
    println("Starting wait for user setting...")
    Await.ready(updatedService, Duration.Inf)
    println("User setting done.")
    println("Starting cluster termination...")
    sm.terminateCluster(id)
    val terminatingState = sm.describeCluster(id).get.state
    terminatingState must (be (Terminated) or be (Terminating))
    println("Starting wait for cluster termination")
    waitForClusterCompletion(id, sm)
    println("Cluster terminated.")
    sm.describeCluster(id).get.state must be (Terminated)
  }

  it must "create persistent hdfs cluster" taggedAs HasExternalDependencies in {
    Await.result(sm.deployPersistentHdfsCluster(), 10 minutes)
  }
}
