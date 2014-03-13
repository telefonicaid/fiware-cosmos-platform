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

package es.tid.cosmos.api.profile

import es.tid.cosmos.common.AlphanumericTokenPattern

/** Represents the API access credentials. */
case class ApiCredentials(apiKey: String, apiSecret: String) {
  ApiCredentials.ApiKeyPattern.requireValid(apiKey)
  ApiCredentials.ApiSecretPattern.requireValid(apiSecret)
}

object ApiCredentials {
  val ApiKeyLength = 20
  val ApiSecretLength = 40
  private val ApiKeyPattern = new AlphanumericTokenPattern("API key", ApiKeyLength)
  private val ApiSecretPattern = new AlphanumericTokenPattern("API secret", ApiSecretLength)

  def random(): ApiCredentials =
    ApiCredentials(ApiKeyPattern.generateRandom(), ApiSecretPattern.generateRandom())
}
