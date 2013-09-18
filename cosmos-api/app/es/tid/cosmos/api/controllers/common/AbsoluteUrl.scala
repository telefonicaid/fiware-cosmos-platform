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

package es.tid.cosmos.api.controllers.common

import play.Play
import play.api.mvc.{RequestHeader, Call}

/**
 * Factory of absolute URLs.
 */
object AbsoluteUrl {
  private def sslEnabled: Boolean =
    Play.application().configuration().getBoolean("application.ssl.enabled", false)

  /**
   * Create an absolute URL.
   *
   * @param call    Reverse route
   * @param request Implicit request to use as base address
   * @return        HTTP or HTTPS URL depending on the configuration
   */
  def apply(call: Call)(implicit request: RequestHeader): String = call.absoluteURL(sslEnabled)
}
