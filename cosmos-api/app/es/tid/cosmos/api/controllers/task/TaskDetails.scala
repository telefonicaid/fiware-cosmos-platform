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

import es.tid.cosmos.api.task.{ImmutableTask, TaskStatus}
import play.api.libs.json._

case class TaskDetails(id: Int, status: TaskStatus, resource: String)

object TaskDetails {
  def apply(details: ImmutableTask): TaskDetails =
    TaskDetails(details.id, details.status, details.resource)

  implicit object TaskDetailsWrites extends Writes[TaskDetails] {
    override def writes(info: TaskDetails): JsValue = Json.obj(
      "id" -> info.id,
      "status" -> info.status.toString,
      "resource" -> info.resource
    )
  }
}
