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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions

class CosmosIT extends FlatSpec with MustMatchers {
  "The Cosmos resource" must "redirect to the doc page" in new WithSampleSessions {
    val res = regUserInGroup.doRequest("/cosmos/v1")
    status(res) must equal (MOVED_PERMANENTLY)
    header("Location", res).get must include ("/doc.html")
  }
}
