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

package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.cosmos.routes
import es.tid.cosmos.api.mocks.WithTestApplication

class AbsoluteUrlTest extends FlatSpec with MustMatchers {

  implicit val request = FakeRequest()

  "Absolute URL factory" must "create HTTP URLs by default" in new WithTestApplication {
    AbsoluteUrl(routes.CosmosResource.get()) must startWith ("http://")
  }

  it must "create HTTPS URLs when configured for it" in
    new WithTestApplication(Map("application.ssl.enabled" -> "true")) {
      AbsoluteUrl(routes.CosmosResource.get()) must startWith ("https://")
    }
}
