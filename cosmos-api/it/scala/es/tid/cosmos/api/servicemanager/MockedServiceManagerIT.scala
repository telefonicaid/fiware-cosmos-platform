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

package es.tid.cosmos.api.servicemanager

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.servicemock.MockedServiceManager
import es.tid.cosmos.servicemanager._

class MockedServiceManagerIT extends FlatSpec with MustMatchers {
  val transitionDelay = 100

  trait Instance {
    val instance = new MockedServiceManager(transitionDelay)
  }

  "A mocked service manager" must "be pre-populated with a cluster" in new Instance {
    instance.clusterIds must have size 1
  }

  it must "create a new cluster in provisioning state and then transition to running state" in new Instance {
    val id = instance.createCluster("small cluster", 10, serviceDescriptions = Seq())
    instance.describeCluster(id).get must have ('state (Provisioning))
    Thread.sleep(2 * transitionDelay)
    instance.describeCluster(id).get must have ('state (Running))
  }

  it must "start terminating a cluster and then transition to terminated state" in new Instance {
    val id: ClusterId = MockedServiceManager.defaultClusterId
    instance.terminateCluster(id)
    instance.describeCluster(id).get must have ('state (Terminating))
    Thread.sleep(2 * transitionDelay)
    instance.describeCluster(id).get must have ('state (Terminated))
  }
}
