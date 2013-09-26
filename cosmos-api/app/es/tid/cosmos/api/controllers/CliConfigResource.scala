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

import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.controllers.common.AbsoluteUrl
import es.tid.cosmos.api.controllers.cosmos.{routes => cosmosRoutes}
import es.tid.cosmos.api.profile.CosmosProfileDao

/** Downloadable configuration file for cosmos-cli */
class CliConfigResource(dao: CosmosProfileDao) extends Controller {

  def generate = Action { implicit request =>
    (for {
      userProfile <- session.userProfile if session.isRegistered
      cosmosProfile <- dao.withConnection { implicit c =>
        dao.lookupByUserId(userProfile.id)
      }
    } yield {
      val config = CliConfig(
        apiCredentials = cosmosProfile.apiCredentials,
        apiUrl = AbsoluteUrl(cosmosRoutes.CosmosResource.version())
      )
      Ok(config.toString).withHeaders(
        "Content-Type" -> "text/yaml",
        "Content-Disposition" -> "attachment; filename=cosmosrc.yaml"
      )
    }).getOrElse(NotFound)
  }
}
