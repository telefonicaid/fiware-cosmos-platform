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

import org.scalatest.matchers.{MatchResult, Matcher}

import es.tid.cosmos.api.profile.Capability.Capability

/** Test helpers to assert */
trait CapabilityMatchers {

  /** Match profiles with a given capability */
  def haveCapability(capability: Capability): Matcher[CosmosProfile] = new Matcher[CosmosProfile] {
    override def apply(profile: CosmosProfile): MatchResult = 
      matchCapability("profile", capability, profile.capabilities)
  }

  /** Match user capabilities containing a specific capability */
  def containCapability(capability: Capability): Matcher[UserCapabilities] =
    new Matcher[UserCapabilities] {
      override def apply(capabilities: UserCapabilities): MatchResult =
        matchCapability("user capabilities", capability, capabilities)
    }

  private def matchCapability(
      subject: String,
      capability: Capability,
      capabilities: UserCapabilities) = MatchResult(
    matches = capabilities.hasCapability(capability),
    failureMessage = s"$subject has capability $capability disabled",
    negatedFailureMessage = s"$subject has capability $capability enabled"
  )
}
