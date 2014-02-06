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
