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
