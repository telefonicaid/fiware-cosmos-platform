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

package es.tid.cosmos.api.controllers.storage

import java.net.URI

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.MaintenanceModeBehaviors
import es.tid.cosmos.api.mocks.WithSampleUsers
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager

class StorageResourceIT
  extends FlatSpec with MustMatchers with OneInstancePerTest with MaintenanceModeBehaviors {

  val request = FakeRequest(GET, "/cosmos/v1/storage")

  "The storage resource" must "be unavailable when no service is active" in new WithSampleUsers {
    val result = route(request.authorizedBy(user1)).get
    status(result) must be (SERVICE_UNAVAILABLE)
  }

  it must "provide the WebHDFS connection details" in new WithSampleUsers {
    services.serviceManager().deployPersistentHdfsCluster()
    val result = route(request.authorizedBy(user1)).get
    status(result) must be (OK)
    val jsonBody = Json.parse(contentAsString(result))
    Json.fromJson[WebHdfsConnection](jsonBody).get must be (WebHdfsConnection(
      location = new URI("web" + MockedServiceManager.PersistentHdfsUrl),
      user = "user1"
    ))
  }

  it must "reject unauthenticated requests" in new WithSampleUsers {
    val result = route(request).get
    status(result) must be (UNAUTHORIZED)
  }

  it must behave like resourceDisabledWhenUnderMaintenance(request)
}
