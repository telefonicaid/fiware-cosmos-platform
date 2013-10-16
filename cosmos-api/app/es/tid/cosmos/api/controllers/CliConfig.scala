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

package es.tid.cosmos.api.controllers

import es.tid.cosmos.api.authorization.ApiCredentials

case class CliConfig(apiCredentials: ApiCredentials, apiUrl: String, sshClient: String = "ssh") {
  override def toString =
    s"""api_key: ${apiCredentials.apiKey}
      |api_secret: ${apiCredentials.apiSecret}
      |api_url: $apiUrl
      |ssh_command: $sshClient
    """.stripMargin
}
