package es.tid.cosmos.admin.profile

import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

trait DefaultProfileCommandsComponent extends ProfileCommandsComponent {
  this: CosmosDataStoreComponent with ServiceManagerComponent =>

  lazy val profileCommands: ProfileCommands = new DefaultProfileCommands(store, serviceManager)
}
