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

package es.tid.cosmos.api

import play.api.{Play, GlobalSettings}
import play.api.Play.current
import com.typesafe.config.ConfigFactory

/**
 * @author sortega
 */
object Global extends GlobalSettings {
  override def getControllerInstance[A](controllerClass: Class[A]): A = {
    val mockServices = ConfigFactory.load().getBoolean("application.mock-services")
    val application = if (Play.isTest || mockServices) TestApplication else ProductionApplication
    application.asInstanceOf[A]
  }
}
