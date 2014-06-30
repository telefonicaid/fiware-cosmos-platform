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

package es.tid.cosmos.api.auth.oauth2

import scala.concurrent.Future

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class AbstractOAuthProviderTest extends FlatSpec with MustMatchers {

  val requiredSettings =
    """
      |name="Any OAuth provider"
      |client.id="client_id"
      |client.secret="client_secret"
    """.stripMargin

  class TestProvider(config: String)
      extends AbstractOAuthProvider("id", ConfigFactory.parseString(requiredSettings + config)) {
    override def requestAccessToken(code: String, redirectUrl: String): Future[String] = ???
    override def authenticationUrl(redirectUrl: String): String = ???
    override protected val profileParser: ProfileParser = null
    override protected def requestProfileResource(token: String): Future[String] = ???
  }

  "Any OAuth provider" must "disable external admin when explicitly disabled" in {
    new TestProvider("externalAdmin.enabled=false").externalAdministrationEnabled must be (false)
  }

  it must "disable external admin when missing related config" in {
    new TestProvider("").externalAdministrationEnabled must be (false)
  }

  it must "enable external admin with the configured password" in {
    val provider = new TestProvider(
      """
        |externalAdmin.enabled=true
        |externalAdmin.password="password"
      """.stripMargin
    )
    provider.externalAdministrationEnabled must be (true)
    provider.adminPassword must be (Some("password"))
  }

  it must "require password to be defined when administration is enabled" in {
    val ex = evaluating {
      new TestProvider("externalAdmin.enabled=true").adminPassword
    } must produce [IllegalArgumentException]
    ex.toString must include ("auth.id.externalAdmin.password is mandatory")
  }
}
