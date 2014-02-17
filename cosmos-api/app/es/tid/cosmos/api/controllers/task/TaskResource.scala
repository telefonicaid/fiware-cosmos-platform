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

import javax.ws.rs.PathParam
import scalaz.Scalaz._

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{CosmosProfile, CosmosProfileDao}
import es.tid.cosmos.api.task.{Task, TaskDao}

@Api(value = "/cosmos/v1/task", listingPath = "/doc/cosmos/v1/task",
  description = "Represents an ongoing operation")
class TaskResource(
    taskDao: TaskDao,
    override val dao: CosmosProfileDao)
  extends Controller with ApiAuthController with JsonController {

  private def requireTaskExists(id: Int): ActionValidation[Task] = taskDao.get(id) match {
    case Some(task) => task.success
    case None => NotFound(Json.toJson(ErrorMessage(s"No task with id $id exists"))).failure
  }

  @ApiOperation(value = "Get task details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.task.TaskDetails")
  @ApiErrors(Array(
    new ApiError(code = 404, reason = "When task ID is unknown")
  ))
  def getDetails(
      @ApiParam(value = "Task identifier", required = true,
        defaultValue = "0")
      @PathParam("id")
      id: String) = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
      task <- requireTaskExists(id.toInt)
      _ <- requireAccessToTask(task, profile)
    } yield Ok(Json.toJson(TaskDetails(task.view)))
  }

  private def requireAccessToTask(task: Task, profile: CosmosProfile): ActionValidation[Unit] = {
    if (task.usersWithAccess.contains(profile.handle)) ().success
    else Unauthorized(Json.toJson(ErrorMessage(s"Cannot access task ${task.id}"))).failure
  }
}
