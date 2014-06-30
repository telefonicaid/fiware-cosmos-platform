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

package es.tid.cosmos.api.profile.dao

import es.tid.cosmos.api.profile._

trait CapabilityDao[Conn] {

  /** Enable some capability to the given user.
    *
    * If the capability was already enabled, nothing is done.
    *
    * @param id           The unique Cosmos ID of the given user.
    * @param capability   The capability to be enabled.
    * @throws CosmosDaoException  When no user has such id
    */
  def enable(id: ProfileId, capability: Capability.Value)(implicit c: Conn): Unit

  /** Disable some capability to the given user.
    *
    * If the capability was already disabled, nothing is done.
    *
    * @param id           The unique Cosmos ID of the given user.
    * @param capability   The capability to be disabled.
    */
  def disable(id: ProfileId, capability: Capability.Value)(implicit c: Conn): Unit

  /** List capabilities for a given profile.
    *
    * @param id  Profile id
    * @return    User capabilities or UntrustedUserCapabilities if the user doesn't exist
    */
  def userCapabilities(id: ProfileId)(implicit c: Conn): UserCapabilities
}
