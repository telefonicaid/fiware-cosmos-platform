package es.tid.cosmos.api.sm

import es.tid.cosmos.servicemanager.ServiceManager

/**
 * @author sortega
 */
trait MockedServiceManagerComponent extends ServiceManagerComponent {
  val serviceManager: ServiceManager = new MockedServiceManager(10000)
}
