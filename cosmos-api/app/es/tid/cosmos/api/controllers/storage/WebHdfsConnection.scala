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

package es.tid.cosmos.api.controllers.storage

import java.net.URI

import play.api.libs.functional.syntax._
import play.api.libs.json._

/**
 * Represents a connection to a WebHDFS resource.
 *
 * @param location url with webhdfs: scheme
 * @param user     username to access the WebHDFS
 */
case class WebHdfsConnection(location: URI, user: String) {
  require(location.getScheme == "webhdfs")
}

object WebHdfsConnection {
  implicit val webHdfsConnectionReads: Reads[WebHdfsConnection] = (
    (__ \ "location").read[String] ~
    (__ \ "user").read[String]
  )((location, user) => WebHdfsConnection(new URI(location), user))

  implicit object WebHdfsConnectionWrites extends Writes[WebHdfsConnection] {
    def writes(connection: WebHdfsConnection): JsValue = Json.obj(
      "location" -> connection.location.toString,
      "user" -> connection.user
    )
  }
}
