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

import play.api.Logger
import play.api.libs.json.Json
import play.api.mvc.{RequestHeader, Result}
import play.api.mvc.Results.InternalServerError

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.controllers.common.ErrorMessage
import es.tid.cosmos.api.oauth2.TuIdOAuthClientComponent
import es.tid.cosmos.platform.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.ProductionServiceManagerComponent

/**
 * Global application settings tied to real services.
 */
object ProductionGlobal extends AbstractGlobal(new Application
  with TuIdOAuthClientComponent
  with ProductionServiceManagerComponent
  with ApplicationConfigComponent) {

  override def onError(request: RequestHeader, ex: Throwable): Result = {
    val message = ErrorMessage("Uncaught exception", ex)
    Logger.error(message.error, ex)
    InternalServerError(Json.toJson(message))
  }
}
