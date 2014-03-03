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

package es.tid.cosmos.api.controllers.cosmos

import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.controllers.pages.{routes => pageRoutes}

/** Cosmos API root */
class CosmosResource extends Controller {
  def get = Action {
    MovedPermanently(pageRoutes.Pages.swaggerUI().url)
  }
}
