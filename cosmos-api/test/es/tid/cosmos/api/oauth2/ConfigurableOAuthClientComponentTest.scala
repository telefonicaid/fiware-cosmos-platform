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

package es.tid.cosmos.api.oauth2

import scala.collection.JavaConversions

import com.typesafe.config.{ConfigFactory, Config}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.ConfigComponent

class ConfigurableOAuthClientComponentTest extends FlatSpec with MustMatchers {

  /**
   * Mix-in of ConfigurableOAuthClientComponent and a dummy test configuration.
   *
   * @constructor
   * @param maybeProvider has the content of config key 'oauth.provider' when defined,
   *                      otherwise the property is not included in the configuration
   */
  class ComponentWithConfig(maybeProvider: Option[String])
    extends ConfigComponent with ConfigurableOAuthClientComponent {

    def config: Config = {
      val configMap = maybeProvider.map(provider =>
        Map("oauth.provider" -> provider)
      ).getOrElse(Map.empty)
      ConfigFactory.parseMap(JavaConversions.mapAsJavaMap(configMap))
    }
  }

  "A configurable OAuth client component" must
    "provide a TU|id client when 'tuid' is configured" in {
      val instance = new ComponentWithConfig(Some("tuid"))
      instance.oAuthClient.getClass must equal (classOf[TuIdOAuthClient])
    }

  it must "provide a GitHub client when 'github' is configured" in {
    val instance = new ComponentWithConfig(Some("github"))
    instance.oAuthClient.getClass must equal (classOf[GitHubOAuthClient])
  }

  it must "throw if an unknown provider is configured" in {
    evaluating {
      new ComponentWithConfig(Some("unknown")).oAuthClient
    } must produce [ExceptionInInitializerError]
  }

  it must "throw if 'oauth.provider' is not configured" in {
      evaluating {
        new ComponentWithConfig(None).oAuthClient
      } must produce [ExceptionInInitializerError]
    }
}
