package es.tid.cosmos.api

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.servicemanager.AmbariServiceManagerComponent
import es.tid.cosmos.platform.manager.ial.serverpool.ServerPoolInfrastructureProviderComponent

/**
 * @author sortega
 */
object ProductionApplication
  extends Application
  with AmbariServiceManagerComponent
  with ServerPoolInfrastructureProviderComponent
