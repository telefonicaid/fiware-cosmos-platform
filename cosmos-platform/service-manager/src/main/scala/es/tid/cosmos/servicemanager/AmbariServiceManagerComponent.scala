package es.tid.cosmos.servicemanager

import es.tid.cosmos.servicemanager.ambari.AmbariServer
import es.tid.cosmos.platform.manager.ial.InfrastructureProviderComponent

/**
 * @author sortega
 */
trait AmbariServiceManagerComponent extends ServiceManagerComponent {
  self: InfrastructureProviderComponent =>

  val serviceManager: ServiceManager = new AmbariServiceManager(
    new AmbariServer("cosmos.local", 8080, "admin", "admin"), infrastructureProvider)
}
