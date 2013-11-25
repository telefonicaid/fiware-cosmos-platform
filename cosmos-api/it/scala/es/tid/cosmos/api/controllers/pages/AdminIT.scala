package es.tid.cosmos.api.controllers.pages

import scala.language.reflectiveCalls

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.{MaintenanceModeBehaviors, AuthBehaviors}
import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo

class AdminIT extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  val path = "/admin"

  "The admin page" must behave like pageDisabledWhenUnderMaintenance(FakeRequest(GET, path))

  it must "redirect not registered users" in new WithSampleSessions {
    unregUser.doRequest(path) must redirectTo ("/register")
    unauthUser.doRequest(path) must redirectTo ("/")
  }

  it must "be not found by users without operator capability" in new WithSampleSessions {
    val response = regUser.doRequest(path)
    status(response) must be (UNAUTHORIZED)
    contentAsString(response) must include ("You are not authorized")
  }

  it must "show the maintenance status" in new WithSampleSessions {
    val response = opUser.doRequest(path)
    status(response) must be (OK)
    contentAsString(response) must include ("Maintenance status")
  }
}
