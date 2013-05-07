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

import scala.Some

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._
import play.api.test.FakeRequest
import play.api.libs.json.Json

import es.tid.cosmos.api.servicemock.MockedServiceManager
import es.tid.cosmos.servicemanager.ClusterId

class ClusterIT extends FlatSpec with MustMatchers with MockedServices {
  val resourcePath = s"/cosmos/cluster/${MockedServiceManager.defaultClusterId.uuid.toString}"

  "Cluster resource" must "list cluster details on GET request" in {
    runWithMockedServices {
      val resource = route(FakeRequest(GET, resourcePath)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = Json.parse(contentAsString(resource))
      (description \ "id").as[String] must equal (MockedServiceManager.defaultClusterId.uuid.toString)
    }
  }

  it must "return 404 on unknown cluster" in {
    val unknownClusterId = ClusterId()
    runWithMockedServices {
      val resource = route(FakeRequest(GET, s"/cosmos/cluster/${unknownClusterId}")).get
      status(resource) must equal (NOT_FOUND)
    }
  }
}
