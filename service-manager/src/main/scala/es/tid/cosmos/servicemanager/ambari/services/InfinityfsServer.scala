package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.configuration.NoConfigurationContribution

object InfinityfsServer extends AmbariServiceDescription with NoConfigurationContribution {
  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.allNodesComponent("INFINITY_HFS_SERVER"))
  override val name: String = "INFINITYFS_SERVER"
}
