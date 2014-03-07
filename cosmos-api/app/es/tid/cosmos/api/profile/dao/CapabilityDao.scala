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
