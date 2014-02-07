package es.tid.cosmos.api.controllers.common

import play.api.mvc.{Action, Controller}
import play.api.test._
import play.api.test.Helpers._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.task.inmemory.InMemoryTaskDao

class TaskControllerIT extends FlatSpec with MustMatchers {

  object TestController extends Controller with TaskController {
    override val taskDao = new InMemoryTaskDao()

    def startTask(resource: String, metadata: String) = Action { request =>
      for {
        _ <- requireNoActiveTask(resource, metadata)
      } yield {
        val task = taskDao.registerTask()
        task.resource = resource
        task.metadata = metadata
        Ok(s"Task $metadata on $resource started")
      }
    }
  }

  val request = FakeRequest(GET, "/sample")

  "A task controller" must "allow task not being active" in {
    val res = TestController.startTask("res1", "task1").apply(request)
    status(res) must be (OK)
    contentAsString(res) must include ("Task task1 on res1 started")
  }

  it must "allow different tasks on same resource and vice versa" in {
    status(TestController.startTask("res1", "task2").apply(request)) must be (OK)
    status(TestController.startTask("res2", "task1").apply(request)) must be (OK)
  }

  it must "reject repeated tasks" in {
    val res = TestController.startTask("res1", "task1").apply(request)
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("already running")
  }
}
