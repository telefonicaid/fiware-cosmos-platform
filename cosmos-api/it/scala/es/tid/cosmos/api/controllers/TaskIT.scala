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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleTasks
import play.api.libs.json.Json
import es.tid.cosmos.api.controllers.task.TaskDetails

class TaskIT extends FlatSpec with MustMatchers {
  "The Task resource" must "return the TaskDetails is the user is authorized" in new WithSampleTasks {
    val res = regUserInGroup.doRequest(s"/cosmos/v1/task/${regUserAuthorizedTask.id}")
    contentAsJson(res) must equal (Json.toJson(TaskDetails(regUserAuthorizedTask.view)))
  }

  it must "return 401 if the user is not authenticated" in new WithSampleTasks {
    val res = unauthUser.doRequest(s"/cosmos/v1/task/${regUserAuthorizedTask.id}")
    status(res) must equal (401)
  }

  it must "return 401 if the user is not authorized to view that task" in new WithSampleTasks {
    val res = regUserInGroup.doRequest(s"/cosmos/v1/task/${inaccesibleTask.id}")
    status(res) must equal (401)
  }

  it must "return 404 if the task does not exist" in new WithSampleTasks {
    val res = regUserInGroup.doRequest(s"/cosmos/v1/task/10000")
    status(res) must equal (404)
  }
}
