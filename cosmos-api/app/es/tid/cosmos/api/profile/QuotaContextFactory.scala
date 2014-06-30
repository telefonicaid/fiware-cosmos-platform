/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
