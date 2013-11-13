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
import es.tid.cosmos.api.mocks.WithSampleUsers
import es.tid.cosmos.api.profile.{UserState, Registration, UserId, CosmosProfileDao}

class ApiAuthControllerIT extends FlatSpec with MustMatchers {

  class TestController(override val dao: CosmosProfileDao) extends ApiAuthController {
    def index() = Action(parse.anyContent) { request =>
      withApiAuth(request) { profile =>
        Ok(s"handle=${profile.handle}")
      }
    }
  }

  def action(dao: CosmosProfileDao, request: Request[AnyContent]): Future[SimpleResult] =
    new TestController(dao).index().apply(request)

  val request: Request[AnyContent] = FakeRequest(GET, "/some/path")
  val registration = Registration("login", "ssh-rsa AAAA login@host", "login@host")

  "The API auth controller" must "not authorize when authorization header is missing" in
    new WithSampleUsers {
      val response = action(dao, request)
      status(response) must be (UNAUTHORIZED)
      contentAsString(response) must include ("Missing authorization header")
    }

  it must "not authorize when authorization header is malformed" in new WithSampleUsers {
    val response = action(dao, authorizedRequest("Basic malformed"))
    status(response) must be (UNAUTHORIZED)
    contentAsString(response) must include ("malformed authorization header")
  }

  it must "not authorize when credentials belong to a non-enabled user" in new WithSampleUsers {
    val profile = dao.withConnection { implicit c =>
      val userId = UserId("db000")
      val profile = dao.registerUserInDatabase(userId, registration)
      dao.setUserState(profile.id, UserState.Disabled)
      profile
    }
    val response = action(dao, authorizedRequest(profile.apiCredentials))
    status(response) must be (UNAUTHORIZED)
  }

  it must "return bad request when credentials are invalid" in new WithSampleUsers {
    val response = action(dao, authorizedRequest(ApiCredentials.random()))
    status(response) must be (UNAUTHORIZED)
    contentAsString(response) must include ("Invalid API credentials")
  }

  it must "succeed when credentials are valid" in new WithSampleUsers {
    val profile = dao.withConnection { implicit c =>
      val userId = UserId("db000")
      val profile = dao.registerUserInDatabase(userId, registration)
      profile
    }
    val response = action(dao, authorizedRequest(profile.apiCredentials))
    status(response) must be (OK)
    contentAsString(response) must include ("handle=login")
  }

  it must "succeed when the user has a session" in new WithSampleSessions {
    val response = action(dao, FakeRequest(GET, "/some/path")
      .withSession(regUser.session.data.toSeq: _*))
    status(response) must be (OK)
    contentAsString(response) must include (s"handle=${regUser.handle}")
  }

  it must "not authorize when the user having a session is not enabled" in new WithSampleSessions {
    val response = action(dao, FakeRequest(GET, "/some/path")
      .withSession(disabledUser.session.data.toSeq: _*))
    status(response) must be (UNAUTHORIZED)
  }

  it must "have preference for authorization header over user session" in
    new WithSampleUsers with WithSampleSessions {
      val apiCredsProfile = dao.withConnection { implicit c =>
        val userId = UserId("db000")
        dao.registerUserInDatabase(userId, registration)
        dao.lookupByUserId(userId).get
      }
      val response = action(dao, authorizedRequest(apiCredsProfile.apiCredentials)
        .withSession(regUser.session.data.toSeq: _*))
      status(response) must be (OK)
      contentAsString(response) must include (s"handle=login")
      contentAsString(response) must not include s"handle=${regUser.handle}"
    }

  private def authorizedRequest(credentials: ApiCredentials): FakeRequest[AnyContent] =
    authorizedRequest(BasicAuth(credentials.apiKey, credentials.apiSecret))

  private def authorizedRequest(authorizationHeader: String): FakeRequest[AnyContent] =
    FakeRequest(
      method = GET,
      uri = "/some/path",
      headers = FakeHeaders(Seq(
        "Authorization" -> Seq(authorizationHeader))),
      body = AnyContentAsEmpty
    )
}
