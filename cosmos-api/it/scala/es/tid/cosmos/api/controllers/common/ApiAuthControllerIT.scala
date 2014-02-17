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

package es.tid.cosmos.api.controllers.common

import scala.concurrent.Future
import scala.language.reflectiveCalls

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc._
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.profile._

class ApiAuthControllerIT extends FlatSpec with MustMatchers {

  trait WithTestController extends WithSampleSessions {
    private val controllerDao = dao

    object TestController extends Controller with ApiAuthController {
      override val dao = controllerDao

      def index() = Action(parse.anyContent) { request =>
        for {
          profile <- requireAuthenticatedApiRequest(request)
        } yield Ok(s"handle=${profile.handle}")
      }
    }

    def action(request: Request[AnyContent]): Future[SimpleResult] =
      TestController.index().apply(request)

    def authorizedRequest(userSession: UserSession): FakeRequest[AnyContent] = {
      val creds = userSession.apiCredentials.get
      authorizedRequest(creds).withSession(userSession.session.data.toSeq: _*)
    }

    def authorizedRequest(creds: ApiCredentials): FakeRequest[AnyContent] =
      authorizedRequest(BasicAuth(creds.apiKey, creds.apiSecret))

    def authorizedRequest(authorizationHeader: String): FakeRequest[AnyContent] =
      request(FakeHeaders(Seq("Authorization" -> Seq(authorizationHeader))))

    def request(headers: FakeHeaders = FakeHeaders(Seq.empty)): FakeRequest[AnyContent] =
      FakeRequest(
        method = GET,
        uri = resource,
        headers = headers,
        body = AnyContentAsEmpty
      )
  }

  val resource = "/some/path"
  val request = FakeRequest(GET, resource)
  val registration = Registration("login", "ssh-rsa AAAA login@host", "login@host")

  "The API auth controller" must "not authorize when authorization header is missing" in
    new WithTestController {
      val res = action(request())
      status(res) must be (UNAUTHORIZED)
      contentAsString(res) must include ("Missing authorization header")
    }

  it must "not authorize when authorization header is malformed" in new WithTestController {
    val res = action(authorizedRequest("Basic malformed"))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("malformed authorization header")
  }

  it must "not authorize when credentials belong to a disabled user" in new WithTestController {
    val res = action(authorizedRequest(
      userWithState(UserState.Disabled).cosmosProfile.apiCredentials))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("User profile is disabled")
  }

  it must "not authorize when credentials belong to a deleted user" in new WithTestController {
    val res = action(authorizedRequest(
          userWithState(UserState.Deleted).cosmosProfile.apiCredentials))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("User profile is deleted")
  }

  it must "not authorize when credentials belong to user being created" in new WithTestController {
    val res = action(authorizedRequest(
      userWithState(UserState.Creating).cosmosProfile.apiCredentials))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("User profile is still being created")
  }

  it must "not authorize when credentials belong to user being deleted" in new WithTestController {
    val res = action(authorizedRequest(
      userWithState(UserState.Deleting).cosmosProfile.apiCredentials))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("User profile is being deleted")
  }

  it must "return bad request when credentials are invalid" in new WithTestController {
    val invalidCreds = ApiCredentials.random()
    val res = action(authorizedRequest(invalidCreds))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("Invalid API credentials")
  }

  it must "succeed when credentials are valid" in new WithTestController {
    val res = action(authorizedRequest(regUser.cosmosProfile.apiCredentials))
    status(res) must be (OK)
    contentAsString(res) must include (s"handle=${regUser.handle}")
  }

  it must "succeed when the user has a session" in new WithTestController {
    val res = action(authorizedRequest(regUser))
    status(res) must be (OK)
    contentAsString(res) must include (s"handle=${regUser.handle}")
  }

  it must "not authorize when the user having a session is not enabled" in new WithTestController {
    status(action(authorizedRequest(userWithState(UserState.Disabled)))) must be (UNAUTHORIZED)
  }

  it must "have preference for authorization header over user session" in
    new WithTestController {
      val requestWithOpUserHeader = authorizedRequest(opUser.cosmosProfile.apiCredentials)
      val requestWithOpUserHeaderAndRegUserSession =
        requestWithOpUserHeader.withSession(regUser.session.data.toSeq: _*)
      val res = action(requestWithOpUserHeaderAndRegUserSession)
      status(res) must be (OK)
      contentAsString(res) must include (s"handle=${opUser.handle}")
      contentAsString(res) must not include s"handle=${regUser.handle}"
    }
}
