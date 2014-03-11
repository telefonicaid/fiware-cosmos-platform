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

package es.tid.cosmos.api.controllers.pages

import scalaz._

import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.PagesAuthController
import es.tid.cosmos.api.profile.{CosmosProfile, Capability}
import es.tid.cosmos.api.profile.Capability.Capability
import es.tid.cosmos.api.profile.dao.ProfileDataStore

/** Administration page used the Cosmos operators */
class AdminPage(
    override val store: ProfileDataStore,
    override val maintenanceStatus: MaintenanceStatus
  ) extends Controller with PagesAuthController with MaintenanceAwareController {

  import Scalaz._

  def show = Action { implicit request =>
    for {
      profiles <- requireUserProfiles(request)
      (_, cosmosProfile) = profiles
      _ <- requireCapability(cosmosProfile, Capability.IsOperator)
    } yield Ok(views.html.admin(
      underMaintenance = maintenanceStatus.isUnderMaintenance,
      tabs = Navigation.OperatorNavigation))
  }

  private def requireCapability(
      profile: CosmosProfile, capability: Capability): ActionValidation[Unit] =
    if (profile.capabilities.hasCapability(capability)) ().success
    else Forbidden(views.html.forbidden()).fail
}
