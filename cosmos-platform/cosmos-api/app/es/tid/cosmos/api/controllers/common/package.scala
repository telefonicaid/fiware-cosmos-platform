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

import play.api.Play
import play.api.Play.current
import play.api.libs.json._


/**
 * @author sortega
 */
package object common {

  /**
   * Formats an internal server exception to JSON.
   * Stack traces are not shown in production mode.
   *
   * @param message One line explanation
   * @param ex      Cause of the internal error
   * @return        Error description
   */
  def formatInternalException(message: String, ex: Throwable): JsValue = {
    val basicMessage = Map("error" -> "%s: %s".format(message, ex.getMessage))
    val errorMessage = if (Play.isProd) basicMessage
    else basicMessage ++ Map("exception" -> ex.getClass.getCanonicalName,
                             "stack_trace" -> ex.getStackTraceString)
    Json.toJson(errorMessage)
  }
}
