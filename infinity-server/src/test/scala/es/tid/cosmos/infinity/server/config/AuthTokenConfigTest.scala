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

package es.tid.cosmos.infinity.server.config

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class AuthTokenConfigTest extends FlatSpec with MustMatchers {

  "Auth token config" must "load from actor system config" in {
    val config = ConfigFactory.parseString(
      """infinity.server {
        |  auth-token {
        |    secret = "c0sm0s"
        |    path = "/private/url/${token}/${expire}${path}"
        |    phrase = "${secret}::${expire}::${path}::${query}"
        |    duration = 6000s
        |  }
        |}
      """.stripMargin)
    val authTokenConfig = AuthTokenConfig.fromConfig(config)
    authTokenConfig.secret must be ("c0sm0s")
    authTokenConfig.pathTemplate must be ("/private/url/${token}/${expire}${path}")
    authTokenConfig.phraseTemplate must be ("${secret}::${expire}::${path}::${query}")
    authTokenConfig.duration must be (6000)
  }

  it must "load from actor system config by default values when missing" in {
    val config = ConfigFactory.parseString(
      """infinity.server {
        |  auth-token {
        |    secret = "c0sm0s"
        |  }
        |}
      """.stripMargin)
    val authTokenConfig = AuthTokenConfig.fromConfig(config)
    authTokenConfig.secret must be ("c0sm0s")
    authTokenConfig.pathTemplate must be ("/p/${token}/${expire}${path}")
    authTokenConfig.phraseTemplate must be ("${secret}${expire}${path}${query}")
    authTokenConfig.duration must be (60)
  }
}
