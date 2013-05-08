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

package es.tid.cosmos.api.servicemock

import play.api.GlobalSettings

/**
 * Custom global Play! settings to override controller instantiation.
 */
object TestGlobal extends GlobalSettings {
  override def getControllerInstance[A](controllerClass: Class[A]): A = {
    TestApplication.asInstanceOf[A]
  }
}
