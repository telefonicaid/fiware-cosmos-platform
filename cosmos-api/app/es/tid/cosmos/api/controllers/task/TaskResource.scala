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

import javax.ws.rs.PathParam
import scalaz.Scalaz._

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.api.task.{Task, TaskDao}

@Api(value = "/cosmos/v1/task", listingPath = "/doc/cosmos/v1/task",
  description = "Represents an ongoing operation")
class TaskResource(override val auth: RequestAuthentication, taskDao: TaskDao)
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
      @ApiParam(value = "Task identifier", required = true, defaultValue = "0")
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
