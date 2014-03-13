package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.configuration.NoConfigurationContribution

object InfinityfsServer extends AmbariService with NoConfigurationContribution {
  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("INFINITY_HFS_MASTER_SERVER"),
    ComponentDescription.slaveComponent("INFINITY_HFS_SLAVE_PROXY")
  )
  override val name: String = "INFINITYFS_SERVER"
}
