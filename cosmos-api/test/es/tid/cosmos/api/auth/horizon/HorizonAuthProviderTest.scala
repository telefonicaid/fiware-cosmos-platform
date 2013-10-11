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

package es.tid.cosmos.api.auth.horizon

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class HorizonAuthProviderTest extends FlatSpec with MustMatchers {

  def providerForConfig(config: String) =
    new HorizonAuthProvider("horizon", ConfigFactory.parseString(config))

  "An horizon auth provider" must "enable the admin api" in {
    val provider = providerForConfig("password=\"secret\"")
    provider.id must be ("horizon")
    provider.adminPassword must be ("secret")
  }

  it must "throw exception when the password is not configured" in {
    val ex = evaluating {
      providerForConfig("")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Missing password configuration")
  }
}