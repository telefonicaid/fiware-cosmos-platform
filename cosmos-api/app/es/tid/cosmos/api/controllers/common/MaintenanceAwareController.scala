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
import play.api.mvc.{Controller, SimpleResult}

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus

trait MaintenanceAwareController extends Controller {
  val maintenanceStatus: MaintenanceStatus

  import Scalaz._

  private val unavailableResource = ServiceUnavailable(
    Json.toJson(Message("Service temporarily in maintenance mode")))
  private val unavailablePage = ServiceUnavailable(views.html.maintenance())

  /** Page validation requiring that the page is not under maintenance */
  protected def requirePageNotUnderMaintenance(): ActionValidation[Unit] =
    requireNotUnderMaintenance(unavailablePage)

  /** Page validation requiring that the resource is not under maintenance */
  protected def requireResourceNotUnderMaintenance(): ActionValidation[Unit] =
    requireNotUnderMaintenance(unavailableResource)

  private def requireNotUnderMaintenance(errorPage: SimpleResult): ActionValidation[Unit] =
    if (maintenanceStatus.underMaintenance) errorPage.fail else ().success
}
