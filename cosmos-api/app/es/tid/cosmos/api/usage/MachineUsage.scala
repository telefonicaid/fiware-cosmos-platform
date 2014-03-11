package es.tid.cosmos.api.usage

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.quota.GlobalGroupQuotas
import es.tid.cosmos.servicemanager.clusters.ClusterId

/** Definition of a DAO responsible for retrieving machine usage information. */
trait MachineUsage {

  /** The total number of machines in the machine pool */
  def machinePoolSize: Int

  /** Get the system-wide quotas at a given point in time. */
  def globalGroupQuotas: GlobalGroupQuotas[ProfileId]

  /** Aggregates resource usage by user profile.
    *
    * @param requestedClusterId the currently provisioning cluster to be filtered out or none
    */
  def usageByProfile(requestedClusterId: Option[ClusterId]): Map[ProfileId, Int]
}
