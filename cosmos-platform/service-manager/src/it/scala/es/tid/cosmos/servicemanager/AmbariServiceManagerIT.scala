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

import org.scalatest.matchers.MustMatchers
import org.scalatest.FlatSpec
import scala.annotation.tailrec
import es.tid.cosmos.servicemanager.ambari.AmbariServer
import es.tid.cosmos.scalarest.FakeInfrastructureProviderComponent

@tailrec
class AmbariServiceManagerIT extends FlatSpec with MustMatchers with FakeInfrastructureProviderComponent {

  def waitForClusterCompletion(id: ClusterId, sm: ServiceManager): ClusterState = {
    val description = sm.describeCluster(id)
    description.get.state match {
      case Provisioning => {
        Thread.sleep(1000)
        waitForClusterCompletion(id, sm)
      }
      case _ => description.get.state
    }
  }

  "Ambari server" should "create server" in {
    val sm = new AmbariServiceManager(new AmbariServer("cosmos.local", 8080, "admin", "admin"), infrastructureProvider)
    try {
      val id = sm.createCluster(name = "test", 1)
      val description = sm.describeCluster(id)
      description.get.state must be (Provisioning)
      val endState = waitForClusterCompletion(id, sm)
      endState must be === (Running)
    } finally {
      sm.close()
    }
  }
}
