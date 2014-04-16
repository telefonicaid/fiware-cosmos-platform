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
    status(regUserInGroup.doRequest(request)) must be (SERVICE_UNAVAILABLE)
  }

  it must "provide the WebHDFS connection details" in new WithSampleSessions {
    mockedServiceManager.defineCluster(MockedServiceManager.PersistentHdfsProps)
    val persistentHdfsCluster = services.serviceManager.describePersistentHdfsCluster().get
    val result = regUserInGroup.doRequest(request)
    status(result) must be (OK)
    val jsonBody = contentAsJson(result)
    val conn = Json.fromJson[WebHdfsConnection](jsonBody)
    conn must not be JsError
    conn.get.location.getScheme must be ("webhdfs")
    conn.get.location.getHost must be (persistentHdfsCluster.master.get.ipAddress)
    conn.get.user must be (regUserInGroup.handle)
  }

  it must "reject unauthenticated requests" in new WithSampleSessions {
    val result = route(request).get
    status(result) must be (UNAUTHORIZED)
  }

  it must behave like resourceDisabledWhenUnderMaintenance(request)
}
