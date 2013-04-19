package es.tid.cosmos.api.sm

import es.tid.cosmos.servicemanager.ServiceManager

/**
 * @author sortega
 */
trait ServiceManagerComponent {
  def serviceManager: ServiceManager
}
