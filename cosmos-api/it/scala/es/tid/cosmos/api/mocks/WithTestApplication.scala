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

class WithTestApplication(additionalConfiguration: Map[String, String] = Map.empty)
  extends WithApplication(new FakeApplication(
    withGlobal = Some(TestGlobal),
    additionalConfiguration = additionalConfiguration
  )) {

  import WithTestApplication.ConfigFileProperty

  def services = TestGlobal.application.services

  override def around[T: AsResult](block: => T): Result = {
    super.around {
      val oldValue = Option(System.getProperty(ConfigFileProperty))
      System.setProperty(ConfigFileProperty, "cosmos-api/it/resources/test.conf")
      val result = block
      oldValue.foreach(v => System.setProperty(ConfigFileProperty, v))
      result
    }
  }
}

private object WithTestApplication {
  val ConfigFileProperty: String = "config.file"
}

