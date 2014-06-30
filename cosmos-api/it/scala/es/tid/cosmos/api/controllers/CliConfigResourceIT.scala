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

package es.tid.cosmos.api.controllers

import scala.language.reflectiveCalls

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo
import es.tid.cosmos.api.controllers.pages.WithSampleSessions

class CliConfigResourceIT extends FlatSpec with MustMatchers {

  "A CLI config resource" must "redirect when not authenticated and registered" in
    new WithSampleSessions {
      unauthUser.doRequest("/cosmosrc") must redirectTo ("/")
      unregUser.doRequest("/cosmosrc") must redirectTo ("/register")
    }

  it must "return a cosmosrc when authenticated" in new WithSampleSessions {
    val response = regUserInGroup.doRequest("/cosmosrc")
    status(response) must be (OK)
    contentAsString(response) must (
      include (s"api_key: ${regUserInGroup.cosmosProfile.apiCredentials.apiKey}") and
      include (s"api_secret: ${regUserInGroup.cosmosProfile.apiCredentials.apiSecret}"))
  }
}
