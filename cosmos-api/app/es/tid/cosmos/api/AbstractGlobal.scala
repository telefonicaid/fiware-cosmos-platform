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

package es.tid.cosmos.api

import scala.concurrent.Future

import play.api.{Logger, GlobalSettings}
import play.api.http.MimeTypes.{JSON, HTML}
import play.api.libs.json.Json
import play.api.mvc.Results._
import play.api.mvc.{SimpleResult, RequestHeader, Controller}

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.controllers.common.ErrorMessage
import es.tid.cosmos.api.controllers.cosmos.routes
import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent

/** Custom global Play! settings to override controller instantiation. */
abstract class AbstractGlobal(val application: Application with CosmosDataStoreComponent)
  extends GlobalSettings {

  override def getControllerInstance[A](controllerClass: Class[A]): A = {
    application.controllers.getOrElse(
      controllerClass.asInstanceOf[Class[Controller]],
      throw new RuntimeException(s"No instance for $controllerClass is registered. " +
        s"Add a binding at ${classOf[Application].getCanonicalName}")
    ).asInstanceOf[A]
  }

  override def onError(request: RequestHeader, ex: Throwable): Future[SimpleResult] = {
    val message = ErrorMessage("Uncaught exception", ex)
    Logger.error(message.error, ex)
    val result = responseType(request) match {
      case HTML => InternalServerError(views.html.error(message))
      case JSON => InternalServerError(Json.toJson(message))
    }
    Future.successful(result)
  }

  private def responseType(request: RequestHeader) = {
    val defaultType = defaultResponseType(request.path)
    if (request.accepts(defaultType)) defaultType
    else if (request.accepts(HTML)) HTML
    else JSON
  }

  private def defaultResponseType(path: String) = {
    val apiBase = routes.CosmosResource.get().url
    if (path.startsWith(apiBase)) JSON else HTML
  }
}
