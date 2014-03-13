package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.{NoParametrization, Service, ComponentDescription}

object InfinityfsServer extends Service with NoParametrization {
  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("INFINITY_HFS_MASTER_SERVER"),
    ComponentDescription.slaveComponent("INFINITY_HFS_SLAVE_PROXY")
  )
  override val name: String = "INFINITYFS_SERVER"
}
