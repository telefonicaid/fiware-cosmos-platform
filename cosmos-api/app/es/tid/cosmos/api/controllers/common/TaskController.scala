package es.tid.cosmos.api.controllers.common

import scalaz.Scalaz

import play.api.libs.json.Json
import play.api.mvc.Controller

import es.tid.cosmos.api.task.{Running, TaskDao}

/** Controller mixin adding a validation to avoid running multiple instances of the same task. */
trait TaskController { this: Controller =>

  protected val taskDao: TaskDao

  protected def requireNoActiveTask(resource: String, metadata: Any): ActionValidation[Unit] = {
    import Scalaz._
    val existsTask = taskDao.list.view
      .filter(_.status == Running)
      .exists(task => task.resource == resource && task.metadata == metadata)
    if (existsTask) taskAlreadyExists.failure
    else ().success
  }

  private val taskAlreadyExists =
    BadRequest(Json.toJson(ErrorMessage(
      "A previous task that does the same thing than the current request is already running." +
        " Please wait for it to finish.")))
}
