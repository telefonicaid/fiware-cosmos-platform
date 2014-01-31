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
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleTasks
import play.api.libs.json.Json
import es.tid.cosmos.api.controllers.task.TaskDetails

class TaskIT extends FlatSpec with MustMatchers {
  "The Task resource" must "return the TaskDetails is the user is authorized" in new WithSampleTasks {
    val res = regUser.doRequest(s"/cosmos/v1/task/${regUserAuthorizedTask.id}")
    contentAsJson(res) must equal (Json.toJson(TaskDetails(regUserAuthorizedTask.view)))
  }

  it must "return 401 if the user is not authenticated" in new WithSampleTasks {
    val res = unauthUser.doRequest(s"/cosmos/v1/task/${regUserAuthorizedTask.id}")
    status(res) must equal (401)
  }

  it must "return 401 if the user is not authorized to view that task" in new WithSampleTasks {
    val res = regUser.doRequest(s"/cosmos/v1/task/${inaccesibleTask.id}")
    status(res) must equal (401)
  }

  it must "return 404 if the task does not exist" in new WithSampleTasks {
    val res = regUser.doRequest(s"/cosmos/v1/task/10000")
    status(res) must equal (404)
  }
}
