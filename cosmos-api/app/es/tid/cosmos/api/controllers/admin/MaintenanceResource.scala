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

package es.tid.cosmos.api.controllers.admin

import scalaz._

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile.{Capability, CosmosProfile}

/** Admin resource representing the maintenance status of the system */
@Api(value = "/cosmos/v1/maintenance", listingPath = "/doc/cosmos/v1/maintenance",
  description = "Maintenance status of the Cosmos platform")
class MaintenanceResource(
    override val auth: RequestAuthentication,
    maintenanceStatus: MaintenanceStatus)
  extends Controller with ApiAuthController with JsonController {

  @ApiOperation(value = "Maintenance status of the system", httpMethod = "GET",
    responseClass = "scala.Boolean")
  def get = Action { request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield currentStatusResponse
  }

  @ApiOperation(value = "Change maintenance status", httpMethod = "PUT",
    notes = "The returned body is the new maintenance status as a bare boolean")
    @ApiErrors(Array(
      new ApiError(code = 400, reason = "Invalid JSON payload"),
      new ApiError(code = 401, reason = "Unauthorized user"),
      new ApiError(code = 403, reason = "Not an operator")
    ))
    @ApiParamsImplicit(Array(
      new ApiParamImplicit(paramType = "body", dataType = "scala.Boolean")
    ))
  def put() = Action(parse.tolerantJson) { request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
      _ <- requireOperatorProfile(profile)
      wantedStatus <- validJsonBody[Boolean](request)
    } yield {
      if (wantedStatus) maintenanceStatus.enterMaintenance()
      else maintenanceStatus.leaveMaintenance()
      currentStatusResponse
    }
  }

  private def requireOperatorProfile(profile: CosmosProfile): ActionValidation[Unit] = {
    import Scalaz._
    if (profile.capabilities.hasCapability(Capability.IsOperator)) ().success
    else Forbidden(Json.toJson(Message("Action reserved to operators"))).failure
  }

  private def currentStatusResponse = Ok(Json.toJson(maintenanceStatus.isUnderMaintenance))
}
