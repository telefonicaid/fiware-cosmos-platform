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
