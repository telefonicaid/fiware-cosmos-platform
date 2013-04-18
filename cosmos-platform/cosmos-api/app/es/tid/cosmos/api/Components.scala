package es.tid.cosmos.api

import es.tid.cosmos.sm.ServiceManager
import es.tid.cosmos.api.mock.MockedServiceManager

/**
 * Dependency configuration
 *
 * @author sortega
 */
trait Components {
  val transitionDelay = 10000
  val serviceManager: ServiceManager = new MockedServiceManager(transitionDelay)
}
