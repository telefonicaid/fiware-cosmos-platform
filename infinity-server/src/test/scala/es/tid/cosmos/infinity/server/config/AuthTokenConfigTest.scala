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
