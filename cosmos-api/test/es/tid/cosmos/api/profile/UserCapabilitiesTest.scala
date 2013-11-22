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

class UserCapabilitiesTest extends FlatSpec with MustMatchers {

  "User capability set" must "accept new capabilities" in {
    val initialCapabilities = UntrustedUserCapabilities
    initialCapabilities.hasCapability(IsSudoer) must be (false)
    val extendedCapabilities = initialCapabilities + IsSudoer
    extendedCapabilities.hasCapability(IsSudoer) must be (true)
  }

  it must "be deprived of capabilities" in {
    val initialCapabilities = UserCapabilities(Set(IsOperator))
    initialCapabilities.hasCapability(IsOperator) must be (true)
    val extendedCapabilities = initialCapabilities - IsOperator
    extendedCapabilities.hasCapability(IsOperator) must be (false)
  }

  "Untrusted users" must "have no capabilities" in {
    Capability.values.foreach { capability =>
      UntrustedUserCapabilities.hasCapability(capability) must be (false)
    }
  }
}
