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

import scalaz._

import play.api.libs.json.Json
import play.api.mvc.{Results, Controller, SimpleResult}

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.profile.{Capability, CosmosProfile}
import es.tid.cosmos.api.profile.Capability._

trait MaintenanceAwareController extends Controller {
  val maintenanceStatus: MaintenanceStatus

  import Scalaz._
  import MaintenanceAwareController._

  /** Page validation requiring that the page is not under maintenance */
  protected def requirePageNotUnderMaintenance(): ActionValidation[Unit] =
    requireNotUnderMaintenance(unavailablePage)

  /** Page validation requiring that the resource is not under maintenance */
  protected def requireResourceNotUnderMaintenance(): ActionValidation[Unit] =
    requireNotUnderMaintenance(unavailableResource)

  private def requireNotUnderMaintenance(errorPage: SimpleResult): ActionValidation[Unit] =
    if (maintenanceStatus.isUnderMaintenance) errorPage.fail else ().success

  /** Validation that succeed if not under maintenance mode or there is an operator profile.
    * @param profile       Profile to check
    * @param jsonResponse  Whether the error message should be encoded in JSON or HTML
    * @return              Success or an error message
    */
  protected def requireNotUnderMaintenanceToNonOperators(
      profile: CosmosProfile,
      jsonResponse: Boolean = true): ActionValidation[Unit] =
    if (!maintenanceStatus.isUnderMaintenance ||
      profile.capabilities.hasCapability(IsOperator)) ().success
    else if (jsonResponse) unavailableResource.failure
    else unavailablePage.failure
}

object MaintenanceAwareController extends Results {
  private val unavailableResource = ServiceUnavailable(
    Json.toJson(Message("Service temporarily in maintenance mode")))
  private lazy val unavailablePage = ServiceUnavailable(views.html.maintenance())
}
