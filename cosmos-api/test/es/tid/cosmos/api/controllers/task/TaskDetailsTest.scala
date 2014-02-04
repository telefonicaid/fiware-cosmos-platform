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

package es.tid.cosmos.api.controllers.task

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.api.task.{Failed, Finished, Running}
import play.api.libs.json._

class TaskDetailsTest extends FlatSpec with MustMatchers {

  "TaskDetails" must "serialize to JSON correctly when Running" in {
    val task = TaskDetails(40, Running, "/cosmos/v1/cluster")
    Json.toJson(task) must be (Json.obj(
      "id" -> 40,
      "status" -> "Running",
      "resource" -> "/cosmos/v1/cluster"
    ))
  }

  it must "serialize to JSON correctly when Finished" in {
    val task = TaskDetails(0, Finished, "")
    Json.toJson(task) must be (Json.obj(
      "id" -> 0,
      "status" -> "Finished",
      "resource" -> ""
    ))
  }

  it must "serialize to JSON correctly when Failed" in {
    val task = TaskDetails(0, Failed("foo"), "bar")
    Json.toJson(task) must be (Json.obj(
      "id" -> 0,
      "status" -> "Failed: foo",
      "resource" -> "bar"
    ))
  }
}
