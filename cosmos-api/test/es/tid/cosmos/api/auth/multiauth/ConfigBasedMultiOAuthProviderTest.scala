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

package es.tid.cosmos.api.auth.multiauth

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ConfigBasedMultiOAuthProviderTest extends FlatSpec with MustMatchers {

  def buildFromConfig(config: String) =
    new ConfigBasedMultiAuthProvider(ConfigFactory.parseString(config))

  "A MultiOAuthProvider" must "fail to initialize when no provider is defined" in {
    val ex = evaluating { buildFromConfig("") } must produce [IllegalArgumentException]
    ex.getMessage must include ("No authentication provider was defined")
  }

  it must "fail to initialize when no provider is enabled" in {
    val ex = evaluating {
      buildFromConfig(
        """
          |auth.default.enabled=false
          |auth.alternative.enabled=false
        """.stripMargin)
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("No authentication provider was enabled. " +
      "Defined providers: default, alternative.")
  }

  it must "consider missing 'enabled' property as false" in {
    val ex = evaluating {
      buildFromConfig(
        """
          |auth.default.enabled=false
          |auth.alternative.name="Provider 1"
        """.stripMargin)
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("No authentication provider was enabled.")
  }

  it must "instantiate enabled providers" in {
    val config = """
      |auth.default.enabled=true
      |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
      |auth.alternative.enabled=false
    """.stripMargin
    val instance = buildFromConfig(config)
    instance.providers.keySet must equal (Set("default"))
    val providerConfig = ConfigFactory.parseString(config).getConfig("auth.default")
    instance.providers("default") must equal (StubProvider("default", providerConfig))
  }

  it must "throw a human-readable explanation when cannot instantiate a provider" in {
    val ex = evaluating {
      buildFromConfig("""
        |auth.default.enabled=true
        |auth.default.class=not.in.classpath.Provider
      """.stripMargin)
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Cannot initialize the 'default' authentication provider, " +
      "check that its configuration ('auth.default.*') is correct")
  }

  it must "support multiple providers" in {
    buildFromConfig("""
      |auth.default.enabled=true
      |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
      |auth.alternative.enabled=true
      |auth.alternative.class=es.tid.cosmos.api.auth.multiauth.StubProvider
    """.stripMargin).providers must have size 2
  }

  it must "support HOCON syntax" in {
    buildFromConfig("""
      |auth.default {
      |  enabled=true,
      |  class=es.tid.cosmos.api.auth.multiauth.StubProvider
      |}
      |auth.alternative.enabled=true
      |auth.alternative.class=es.tid.cosmos.api.auth.multiauth.StubProvider
    """.stripMargin).providers must have size 2
  }

  it must "disable token authentication when missing tokenAuth config section" in {
    buildFromConfig("""
      |auth.default.enabled=true
      |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
    """.stripMargin).tokenAuthenticationProvider must be ('empty)
  }

  it must "disable token authentication when enabled property is false" in {
    buildFromConfig(
      """
        |auth.default.enabled=true
        |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
        |tokenAuth {
        |  enabled=false
        |  provider="default"
        |}
      """.stripMargin).tokenAuthenticationProvider must be ('empty)
  }

  it must "enable token authentication against the configured provider" in {
    val tokenProvider = buildFromConfig(
      """
        |auth.default.enabled=true
        |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
        |tokenAuth {
        |  enabled=true
        |  provider="default"
        |}
      """.stripMargin).tokenAuthenticationProvider
    tokenProvider must not be 'empty
    tokenProvider.get.name must be ("default")
  }

  it must "fail to initialize when an unknown provider is configured for token authentication" in {
    val ex = evaluating {
      buildFromConfig(
        """
          |auth.default.enabled=true
          |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
          |tokenAuth {
          |  enabled=true
          |  provider="unknown"
          |}
        """.stripMargin)
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Cannot use 'unknown' since it is not an enabled, OAuth provider")
  }

  it must "fail to initialize when a disabled provider is configured for token authentication" in {
    val ex = evaluating {
      buildFromConfig(
        """
          |auth.default.enabled=true
          |auth.default.class=es.tid.cosmos.api.auth.multiauth.StubProvider
          |auth.disabled.enabled=false
          |auth.disabled.class=es.tid.cosmos.api.auth.multiauth.StubProvider
          |tokenAuth {
          |  enabled=true
          |  provider="disabled"
          |}
        """.stripMargin)
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Cannot use 'disabled' since it is not an enabled, OAuth provider")
  }
}
