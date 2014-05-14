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
 * Represents a connection to an Infinity resource.
 *
 * @param location url with infinity: scheme
 * @param user     username to access Infinity
 */
case class InfinityConnection(location: URI, user: String) {
  require(location.getScheme == "infinity")
}

object InfinityConnection {
  implicit val infinityConnectionReads: Reads[InfinityConnection] = (
    (__ \ "location").read[String] ~
    (__ \ "user").read[String]
  )((location, user) => InfinityConnection(new URI(location), user))

  implicit object InfinityConnectionWrites extends Writes[InfinityConnection] {
    def writes(connection: InfinityConnection): JsValue = Json.obj(
      "location" -> connection.location.toString,
      "user" -> connection.user
    )
  }
}
