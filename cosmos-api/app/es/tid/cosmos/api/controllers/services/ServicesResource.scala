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

package es.tid.cosmos.api.controllers.services

import com.wordnik.swagger.annotations.{ApiOperation, Api}
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.common.JsonController
import es.tid.cosmos.servicemanager.ServiceManager

/**
 * Resource that represents the set of services that can be installed in clusters.
 */
@Api(value = "/cosmos/v1/services", listingPath = "/doc/cosmos/v1/services",
  description = "Represents all the services that can optionally be installed in a cluster")
class ServicesResource(serviceManager: ServiceManager) extends JsonController {
  /**
   * List cluster services.
   */
  @ApiOperation(value = "List services", httpMethod = "GET", responseClass = "Seq[String]")
  def list = Action { implicit request =>
      Ok(Json.toJson(serviceManager.services.map(_.name)))
  }
}
