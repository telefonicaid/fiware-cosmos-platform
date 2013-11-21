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

object Capability extends Enumeration {
  type Capability = Value
  val IsSudoer = Value("is_sudoer")
}

/** Represents a the capabilities of an user.
  *
  * @param capabilities   A set defining the capabilities of the user.
  */
case class UserCapabilities(capabilities: Set[Capability.Value]) {

  def + (capability: Capability.Value): UserCapabilities =
    copy(capabilities = capabilities + capability)

  def - (capability: Capability.Value): UserCapabilities =
    copy(capabilities = capabilities - capability)

  def hasCapability(capability: Capability.Value): Boolean = capabilities.contains(capability)
}

/** The capabilities of an untrusted user. */
object UntrustedUserCapabilities extends UserCapabilities(Set.empty)
