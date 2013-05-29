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

package es.tid.cosmos.api.authorization

import java.security.SecureRandom

/**
 * Represents the api access credentials
 */
case class ApiCredentials(apiKey: String, apiSecret: String) {
  require(apiKey.length == ApiCredentials.ApiIdLength,
    s"API identifier must have a length of ${ApiCredentials.ApiIdLength} " +
      s"but got ${apiKey.length}: $apiKey")
  require(apiSecret.length == ApiCredentials.ApiSecretLength,
    s"API secret must have a length of ${ApiCredentials.ApiSecretLength} " +
      s"but got ${apiSecret.length}: $apiSecret")
}

object ApiCredentials {
  val ApiIdLength = 40
  val ApiSecretLength = 40

  def random(): ApiCredentials =
    ApiCredentials(randomToken(ApiIdLength), randomToken(ApiSecretLength))

  private def randomToken(length: Int): String = {
    val r: SecureRandom = new java.security.SecureRandom()
    val tokenCharacters = "0123456789abcdefghijklmnopqrstuvwzABCDEFGHIJKLMNOPQRSTUVWZ"
    def randomChar = tokenCharacters.charAt(r.nextInt(tokenCharacters.length))
    (1 to length).map(_ => randomChar).mkString("")
  }
}
