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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.Capability._

class UserCapabilitiesTest extends FlatSpec with MustMatchers with CapabilityMatchers {

  "User capability set" must "accept new capabilities" in {
    val initialCapabilities = UntrustedUserCapabilities
    initialCapabilities must not(containCapability (IsSudoer))
    val extendedCapabilities = initialCapabilities + IsSudoer
    extendedCapabilities must containCapability (IsSudoer)
  }

  it must "be deprived of capabilities" in {
    val initialCapabilities = UserCapabilities(Set(IsOperator))
    initialCapabilities must containCapability (IsOperator)
    val reducedCapabilities = initialCapabilities - IsOperator
    reducedCapabilities must not(containCapability(IsOperator))
  }

  "Untrusted users" must "have no capabilities" in {
    Capability.values.foreach { capability =>
      UntrustedUserCapabilities must not(containCapability(capability))
    }
  }
}
