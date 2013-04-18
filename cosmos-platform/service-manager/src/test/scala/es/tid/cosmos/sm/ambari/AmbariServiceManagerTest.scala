package es.tid.cosmos.sm.ambari

import es.tid.cosmos.sm._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

/**
 * @author sortega
 */
class AmbariServiceManagerTest extends FlatSpec with MustMatchers {

  val instance = new AmbariServiceManager

  "Ambari service manager" must "throw unsupported exceptions until being implemented" in {
    intercept[ServiceException] { instance.clusterIds }
    intercept[ServiceException] { instance.createCluster("name", 100) }
    intercept[ServiceException] { instance.describeCluster("id") }
    intercept[ServiceException] { instance.terminateCluster("id") }
  }
}
