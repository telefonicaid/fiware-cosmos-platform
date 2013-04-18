package es.tid.cosmos.api.mock

import org.scalatest.matchers.MustMatchers
import org.scalatest.FlatSpec
import es.tid.cosmos.sm.{Provisioning, Running, Terminating, Terminated}

/**
 * @author sortega
 */
class MockedServiceManagerTest extends FlatSpec with MustMatchers {

  val transitionDelay = 100

  trait Instance {
    val instance = new MockedServiceManager(transitionDelay)
  }

  "A mocked service manager" must "be pre-populated with a cluster" in new Instance {
    instance.clusterIds must have size 1
  }

  it must "create a new cluster in provisioning state and then transition to running state" in new Instance {
    val id = instance.createCluster("small cluster", 10)
    instance.describeCluster(id).get must have ('state (Provisioning))
    Thread.sleep(2 * transitionDelay)
    instance.describeCluster(id).get must have ('state (Running))
  }

  it must "start terminating a cluster and then transition to terminated state" in new Instance {
    instance.terminateCluster(instance.defaultClusterId)
    instance.describeCluster(instance.defaultClusterId).get must have ('state (Terminating))
    Thread.sleep(2 * transitionDelay)
    instance.describeCluster(instance.defaultClusterId).get must have ('state (Terminated))
  }
}
