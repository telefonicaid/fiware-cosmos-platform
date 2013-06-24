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

package es.tid.cosmos.api.controllers.cosmos

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.api.controllers.cosmos.ApiDescription.ApiDescriptionWrites

class ApiDescriptionTest extends FlatSpec with MustMatchers {
  "API descriptions" must "be serializable to JSON" in {
     ApiDescriptionWrites.writes(ApiDescription("10.2-rc1")) must be (
       Json.obj("version" -> "10.2-rc1"))
  }
}
