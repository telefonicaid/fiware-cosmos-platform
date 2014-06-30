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

object Capability extends Enumeration {
  type Capability = Value

  /** Enable the user to execute {{{sudo}}} on Cosmos clusters. */
  val IsSudoer = Value("is_sudoer")

  /** Enable the user for administrative tasks on the platform.  */
  val IsOperator = Value("is_operator")
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
