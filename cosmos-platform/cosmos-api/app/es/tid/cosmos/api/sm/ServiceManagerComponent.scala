package es.tid.cosmos.api.sm

import es.tid.cosmos.sm.ServiceManager

/**
 * @author sortega
 */
trait ServiceManagerComponent {
  def serviceManager: ServiceManager
}
