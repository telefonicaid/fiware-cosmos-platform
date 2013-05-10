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

import play.api.GlobalSettings
import play.api.mvc.Controller

import es.tid.cosmos.api.controllers.Application

/**
 * Custom global Play! settings to override controller instantiation.
 */
abstract class AbstractGlobal(application: Application) extends GlobalSettings {
  override def getControllerInstance[A](controllerClass: Class[A]): A = {
    application.controllers(controllerClass.asInstanceOf[Class[Controller]]).asInstanceOf[A]
  }
}
