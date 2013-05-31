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

import play.api.Play
import play.api.Play.current
import play.api.libs.json.{JsValue, Json}
import play.api.mvc.{RequestHeader, Result}
import play.api.mvc.Results.InternalServerError

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.oauth2.TuIdOAuthClientComponent
import es.tid.cosmos.platform.ial.serverpool.ServerPoolInfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ambari.AmbariServiceManagerComponent

/**
 * Global application settings tied to real services.
 */
object ProductionGlobal extends AbstractGlobal(new Application
  with TuIdOAuthClientComponent
  with AmbariServiceManagerComponent
  with ServerPoolInfrastructureProviderComponent
  with PlayConfigComponent) {

  override def onError(request: RequestHeader, ex: Throwable): Result = {
    InternalServerError(formatInternalException(ex.getMessage, ex))
  }

  /**
   * Formats an internal server exception to JSON.
   * Stack traces are not shown in production mode.
   *
   * @param message One line explanation
   * @param ex      Cause of the internal error
   * @return        Error description
   */
  private def formatInternalException(message: String, ex: Throwable): JsValue = {
    val basicMessage = Map("error" -> s"$message: ${ex.getMessage}")
    val errorMessage =
      if (Play.isProd) basicMessage
      else basicMessage ++ Map("exception" -> ex.getClass.getCanonicalName,
                               "stack_trace" -> ex.getStackTraceString)
    Json.toJson(errorMessage)
  }
}
