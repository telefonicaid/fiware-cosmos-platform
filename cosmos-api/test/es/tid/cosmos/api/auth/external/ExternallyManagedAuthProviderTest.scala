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

package es.tid.cosmos.api.auth.external

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ExternallyManagedAuthProviderTest extends FlatSpec with MustMatchers {

  def providerForConfig(config: String) =
    new ExternallyManagedAuthProvider("external", ConfigFactory.parseString(config))

  "An external auth provider" must "enable the admin api" in {
    val provider = providerForConfig("password=\"secret\"")
    provider.id must be ("external")
    provider.externalAdministrationEnabled must be (true)
    provider.adminPassword must be (Some("secret"))
  }

  it must "throw exception when the password is not configured" in {
    val ex = evaluating {
      providerForConfig("")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Missing password configuration")
  }
}
