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
import play.api.libs.json.Json
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.Global
import es.tid.cosmos.api.controllers.clusters.CreateClusterParams

class ClustersIT extends FlatSpec with MustMatchers {
  "The clusters resource" must "list all existing clusters" in {
    running(FakeApplication(withGlobal = Some(Global))) {
      val resource = route(FakeRequest(GET, "/cosmos/cluster")).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
    }
  }

  it must "start a new cluster" in {
    running(FakeApplication(withGlobal = Some(Global))) {
      val resource = route(FakeRequest(POST, "/cosmos/cluster")
        .withJsonBody(Json.toJson(CreateClusterParams("cluster_new", 120)))).get
      status(resource) must equal (CREATED)
      contentType(resource) must be (Some("application/json"))
      val location = header("Location", resource)
      location must be ('defined)
      contentAsString(resource) must include (location.get)
    }
  }

  it must "reject cluster creation with invalid payload" in {
    running(FakeApplication(withGlobal = Some(Global))) {
      val resource = route(FakeRequest(POST, "/cosmos/cluster")
        .withJsonBody(Json.obj("invalid" -> "json"))).get
      status(resource) must equal (BAD_REQUEST)
    }
  }
}
