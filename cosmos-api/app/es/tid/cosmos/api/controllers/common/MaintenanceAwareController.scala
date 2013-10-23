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

import scala.concurrent.Future

import play.api.libs.json.Json
import play.api.mvc.{Controller, SimpleResult}

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus

trait MaintenanceAwareController extends Controller {
  val maintenanceStatus: MaintenanceStatus

  private val unavailableResource = ServiceUnavailable(
    Json.toJson(Message("Service temporarily in maintenance mode")))
  private val unavailablePage = ServiceUnavailable(views.html.maintenance())

  protected def unlessResourceUnderMaintenance(block: => SimpleResult): SimpleResult =
    if (maintenanceStatus.underMaintenance) unavailableResource else block

  protected def unlessResourceUnderMaintenance(block: => Future[SimpleResult]): Future[SimpleResult] =
    if (maintenanceStatus.underMaintenance) Future.successful(unavailableResource) else block

  protected def unlessPageUnderMaintenance(block: => SimpleResult): SimpleResult =
    if (maintenanceStatus.underMaintenance) unavailablePage else block

  protected def unlessPageUnderMaintenance(block: => Future[SimpleResult]): Future[SimpleResult] =
    if (maintenanceStatus.underMaintenance) Future.successful(unavailablePage) else block
}
