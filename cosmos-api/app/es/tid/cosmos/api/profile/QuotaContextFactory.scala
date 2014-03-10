/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.api.profile

import es.tid.cosmos.api.quota.QuotaContext
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.api.usage.MachineUsage

/** Factory of quota contexts that make use of a [[es.tid.cosmos.api.usage.MachineUsage]]
  * for information retrieval.
  */
class QuotaContextFactory(usageDao: MachineUsage) {

  /** Create a new quota context.
    *
    * @param requestedClusterId the currently provisioning cluster to be filtered out or none
    * @return                   the created quota context
    */
  def apply(requestedClusterId: Option[ClusterId] = None) =
    QuotaContext(
      usageDao.machinePoolSize,
      usageDao.globalGroupQuotas,
      usageDao.usageByProfile(requestedClusterId)
    )
}
