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
