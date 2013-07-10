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

import org.specs2.execute.{Result, AsResult}
import play.api.test.{WithApplication, FakeApplication}

import es.tid.cosmos.api.AbstractGlobal

class WithTestApplication(
    additionalConfiguration: Map[String, String] = Map.empty,
    global: AbstractGlobal = new TestGlobal
  ) extends WithApplication(new FakeApplication(
    withGlobal = Some(global),
    additionalConfiguration = additionalConfiguration
  )) {

  import WithTestApplication.ConfigFileProperty

  def services = global.application.services
  lazy val dao = global.application.dao

  override def around[T: AsResult](block: => T): Result = {
    super.around {
      val oldValue = Option(System.getProperty(ConfigFileProperty))
      try {
        System.setProperty(ConfigFileProperty, "cosmos-api/it/resources/test.conf")
        block
      } finally {
        oldValue.map(System.setProperty(ConfigFileProperty, _))
          .getOrElse(System.clearProperty(ConfigFileProperty))
      }
    }
  }
}

private object WithTestApplication {
  val ConfigFileProperty: String = "config.file"
}

