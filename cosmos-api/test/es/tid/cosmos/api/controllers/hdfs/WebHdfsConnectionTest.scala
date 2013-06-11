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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json
import java.net.URI

class WebHdfsConnectionTest extends FlatSpec with MustMatchers {

  "WebHDFS connection details" must "require locations with 'webhdfs' scheme" in {
    evaluating {
      WebHdfsConnection(location = new URI("ftp://host:8080/path"), user = "user")
    } must produce [IllegalArgumentException]
  }

  it must "be serializable to JSON" in {
    val connection = WebHdfsConnection(location = new URI("webhdfs://localhost/"), user = "jsmith")
    Json.toJson(connection) must be (Json.obj(
      "location" -> "webhdfs://localhost/",
      "user" -> "jsmith"
    ))
  }
}
