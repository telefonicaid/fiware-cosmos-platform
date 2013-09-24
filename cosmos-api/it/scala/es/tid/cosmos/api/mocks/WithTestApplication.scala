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

import scala.Some

import play.core.DevSettings
import play.api.test.{WithApplication, FakeApplication}

import es.tid.cosmos.api.AbstractGlobal

class WithTestApplication(
    additionalConfiguration: Map[String, String] = Map.empty,
    val playGlobal: AbstractGlobal = new TestGlobal
  ) extends WithApplication(WithTestApplication.buildApp(additionalConfiguration, playGlobal)) {

  lazy val dao = playGlobal.application.dao

  def services = playGlobal.application.services
}

object WithTestApplication {

  private def buildApp(additionalConfiguration: Map[String, String], global: AbstractGlobal) =
    new FakeApplication(
      additionalConfiguration = additionalConfiguration,
      withGlobal = Some(global)
    ) with DevSettings {
      lazy val devSettings = Map("config.file" -> "cosmos-api/it/resources/application.conf")
    }
}
