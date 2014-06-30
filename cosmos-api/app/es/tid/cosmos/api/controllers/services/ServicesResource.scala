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

package es.tid.cosmos.api.controllers.services

import com.wordnik.swagger.annotations.{ApiOperation, Api}
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.controllers.common.JsonController
import es.tid.cosmos.servicemanager.ServiceManager

/** Resource that represents the set of services that can be installed in clusters. */
@Api(value = "/cosmos/v1/services", listingPath = "/doc/cosmos/v1/services",
  description = "Represents all the services that can optionally be installed in a cluster")
class ServicesResource(serviceManager: ServiceManager) extends Controller with JsonController {

  /**List cluster services. */
  @ApiOperation(value = "List services", httpMethod = "GET",
    notes = "The response is a bare array of strings (e.g. [\"service1\", \"service2\"])",
    responseClass = "java.lang.String", multiValueResponse = true)
  def list = Action { implicit request =>
    Ok(Json.toJson(serviceManager.optionalServices.map(_.name)))
  }
}
