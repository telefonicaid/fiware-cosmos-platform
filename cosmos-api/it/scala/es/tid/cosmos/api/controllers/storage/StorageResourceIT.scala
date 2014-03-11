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

import scala.concurrent.duration._
import scala.language.postfixOps

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsError, Json}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.MaintenanceModeBehaviors
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager

class StorageResourceIT
  extends FlatSpec with MustMatchers with OneInstancePerTest with MaintenanceModeBehaviors {

  val request = FakeRequest(GET, "/cosmos/v1/storage")
  val clusterCreationTimeout = 2.seconds

  "The storage resource" must "be unavailable when no service is active" in new WithSampleSessions {
    status(regUser.doRequest(request)) must be (SERVICE_UNAVAILABLE)
  }

  it must "provide the WebHDFS connection details" in new WithSampleSessions {
    mockedServiceManager.defineCluster(MockedServiceManager.PersistentHdfsProps)
    val persistentHdfsCluster = services.serviceManager.describePersistentHdfsCluster().get
    val result = regUser.doRequest(request)
    status(result) must be (OK)
    val jsonBody = contentAsJson(result)
    val conn = Json.fromJson[WebHdfsConnection](jsonBody)
    conn must not be JsError
    conn.get.location.getScheme must be ("webhdfs")
    conn.get.location.getHost must be (persistentHdfsCluster.master.get.ipAddress)
    conn.get.user must be ("reguser")
  }

  it must "reject unauthenticated requests" in new WithSampleSessions {
    val result = route(request).get
    status(result) must be (UNAUTHORIZED)
  }

  it must behave like resourceDisabledWhenUnderMaintenance(request)
}
