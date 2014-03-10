package es.tid.cosmos.api.usage

import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

trait DynamicMachineUsageComponent extends MachineUsageComponent {
  this: CosmosDataStoreComponent with ServiceManagerComponent =>

  override lazy val machineUsage: MachineUsage = new DynamicMachineUsage(store, serviceManager)
}
