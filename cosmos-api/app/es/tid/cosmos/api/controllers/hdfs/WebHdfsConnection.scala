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

package es.tid.cosmos.api.controllers.hdfs

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
