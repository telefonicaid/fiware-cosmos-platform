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

package es.tid.cosmos.api.mocks

import play.api.test.{FakeApplication, WithApplication}
import play.api.test.Helpers._

class WithInMemoryDatabase(additionalConfiguration: Map[String, String] = Map.empty)
  extends WithApplication(
    FakeApplication(
      withGlobal = Some(TestGlobal),
      additionalConfiguration =
        Map("application.secret" -> "appsecret", "config.resource" -> "test.conf") ++
        inMemoryDatabase(name="default", options=Map("MODE" -> "MYSQL")) ++
        additionalConfiguration
    )) {
  def services = TestGlobal.application.services
}
