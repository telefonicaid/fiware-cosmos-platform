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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.api.authorization.ApiCredentials

class CliConfigTest extends FlatSpec with MustMatchers {

  "A CLI config" must "be printable to string" in {
    val credentials = ApiCredentials(
      "01234567890123456789", "0123456789012345678901234567890123456789")
    CliConfig(credentials, "http://host/api/v1").toString.trim must be (
      """api_key: 01234567890123456789
        |api_secret: 0123456789012345678901234567890123456789
        |api_url: http://host/api/v1
        |ssh_command: ssh
      """.stripMargin.trim)
  }
}
