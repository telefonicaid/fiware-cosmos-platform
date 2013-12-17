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
