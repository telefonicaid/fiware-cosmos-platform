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

package es.tid.cosmos.api.controllers

import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.cosmos.{routes => cosmosRoutes}
import es.tid.cosmos.api.controllers.common.auth.PagesAuthController
import es.tid.cosmos.api.profile.CosmosDao

/** Downloadable configuration file for cosmos-cli */
class CliConfigResource(override val dao: CosmosDao)
  extends Controller with PagesAuthController {

  def generate = Action { implicit request =>
    for {
      profiles <- requireUserProfiles(request)
      (_, cosmosProfile) = profiles
    } yield {
      val config = CliConfig(
        apiCredentials = cosmosProfile.apiCredentials,
        apiUrl = AbsoluteUrl(cosmosRoutes.CosmosResource.get())
      )
      Ok(config.toString).withHeaders(
        "Content-Type" -> "text/yaml",
        "Content-Disposition" -> "attachment; filename=cosmosrc.yaml"
      )
    }
  }
}
