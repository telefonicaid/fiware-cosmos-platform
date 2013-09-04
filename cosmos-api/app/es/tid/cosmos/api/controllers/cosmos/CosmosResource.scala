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

import com.wordnik.swagger.annotations.{ApiOperation, Api}
import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.controllers.pages.{routes => pageRoutes}

/**
 * Root API resource
 */
@Api(value = "/cosmos/v1", listingPath = "/doc/cosmos/v1", description = "API root")
class CosmosResource extends Controller {
  @ApiOperation(value = "The API root", httpMethod = "GET",
    notes = "Root API resource")
  def version = Action {
    MovedPermanently(pageRoutes.Pages.swaggerUI().url)
  }
}
