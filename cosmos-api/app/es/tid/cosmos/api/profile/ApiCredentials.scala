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

import java.security.SecureRandom

/** Represents the api access credentials. */
case class ApiCredentials(apiKey: String, apiSecret: String) {
  require(apiKey.length == ApiCredentials.ApiKeyLength,
    s"API identifier must have a length of ${ApiCredentials.ApiKeyLength} " +
      s"but got ${apiKey.length}: $apiKey")
  require(apiSecret.length == ApiCredentials.ApiSecretLength,
    s"API secret must have a length of ${ApiCredentials.ApiSecretLength} " +
      s"but got ${apiSecret.length}: $apiSecret")
}

object ApiCredentials {
  val ApiKeyLength = 20
  val ApiSecretLength = 40

  def random(): ApiCredentials =
    ApiCredentials(randomToken(ApiKeyLength), randomToken(ApiSecretLength))

  private def randomToken(length: Int): String = {
    val r = new SecureRandom()
    val tokenCharacters = "0123456789abcdefghijklmnopqrstuvwzABCDEFGHIJKLMNOPQRSTUVWZ"
    def randomChar = tokenCharacters.charAt(r.nextInt(tokenCharacters.length))
    List.fill(length)(randomChar).mkString("")
  }
}
