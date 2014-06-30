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

package es.tid.cosmos.api.controllers

import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.cosmos.{routes => cosmosRoutes}
import es.tid.cosmos.api.controllers.common.auth.PagesAuthController
import es.tid.cosmos.api.profile.dao.ProfileDataStore

/** Downloadable configuration file for cosmos-cli */
class CliConfigResource(override val store: ProfileDataStore)
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
