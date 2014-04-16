/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
