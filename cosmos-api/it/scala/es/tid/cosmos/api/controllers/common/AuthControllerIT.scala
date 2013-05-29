package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.Play.current
import play.api.db.DB
import play.api.mvc._
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.MockedServices
import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.api.controllers.pages.Registration
import es.tid.cosmos.api.controllers.pages.CosmosProfile

class AuthControllerIT extends FlatSpec with MustMatchers with MockedServices {

  object TestController extends AuthController {
    def index() = Action(parse.anyContent) { request =>
      Authenticated(request) { profile =>
        Ok(s"handle=${profile.handle}")
      }
    }
  }

  val action: Action[AnyContent] = TestController.index()
  val request: Request[AnyContent] = FakeRequest(GET, "/some/path")

  "Auth controller" must "return bad request when authorization header is missing" in {
    runWithMockedServices {
      status(action(request)) must be (BAD_REQUEST)
    }
  }

  it must "return bad request when credentials are invalid" in {
    runWithMockedServices {
      status(action(authorizedRequest(ApiCredentials.random()))) must be (UNAUTHORIZED)
    }
  }

  it must "succeed when credentials are valid" in {
    runWithMockedServices {
      DB.withConnection { implicit c =>
        CosmosProfileDao.registerUserInDatabase("db000", Registration("login", "pk"))
        val profile: CosmosProfile = CosmosProfileDao.lookupByUserId("db000").get
        val result: Result = action(authorizedRequest(profile.apiCredentials))
        status(result) must be (OK)
        contentAsString(result) must include ("handle=login")
      }
    }
  }

  private def authorizedRequest(credentials: ApiCredentials): Request[AnyContent] =
    FakeRequest(
      method = GET,
      uri = "/some/path",
      headers = FakeHeaders(Seq(
        "Authorization" -> Seq(BasicAuth(credentials.apiKey, credentials.apiSecret)))),
      body = AnyContentAsEmpty
    )
}
