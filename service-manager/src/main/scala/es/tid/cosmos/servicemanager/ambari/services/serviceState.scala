package es.tid.cosmos.servicemanager.ambari.services

/**
 * Representation of the state of Ambari services.
 */
sealed trait ServiceState

case object StartedService extends ServiceState {
  override def toString = "STARTED"
}

case object InstalledService extends ServiceState {
  override def toString = "INSTALLED"
}