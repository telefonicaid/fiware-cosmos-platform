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

package es.tid.cosmos.api.controllers

import org.apache.commons.lang3.StringEscapeUtils
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc.Session
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo
import es.tid.cosmos.api.controllers.pages.{CosmosSession, Registration}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.mocks.oauth2.MockOAuthConstants
import es.tid.cosmos.api.oauth2.UserProfile
import es.tid.cosmos.api.profile.CosmosProfileDao

class PagesIT extends FlatSpec with MustMatchers {

  "A non registered user" must "be redirected to registration when having a valid OAuth code" in
    new WithTestApplication {
      val redirection = oauthRedirectionWithCode(MockOAuthConstants.GrantedCode)
      redirection must redirectTo ("/")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
      cosmosSession must not be 'registered
    }

  it must "be registered upon adding the necessary registration information" in
    new WithTestApplication {
      val nonRegistered = Session().setUserProfile(MockOAuthConstants.User101)
      val registrationPage = route(withSession(FakeRequest(GET, "/"), nonRegistered)).get
      val url = registrationUrl(contentAsString(registrationPage))
      val regRedirection = route(withSession(FakeRequest(POST, url), nonRegistered)
        .withFormUrlEncodedBody(
          "handle" -> "user1",
          "publicKey" -> "SSH_PUBLIC_KEY"
        )).get
      regRedirection must redirectTo ("/")
      val registeredCosmosSession: CosmosSession = session(regRedirection)
      registeredCosmosSession must be ('authenticated)
      registeredCosmosSession must be ('registered)
    }

  it must "not be authenticated after cancelling the registration process" in
    new WithTestApplication {
      val nonRegistered = Session().setUserProfile(MockOAuthConstants.User101)
      val registrationPage = route(withSession(FakeRequest(GET, "/"), nonRegistered)).get
      val url = cancelRegistrationUrl(contentAsString(registrationPage))
      val redirection =  route(withSession(FakeRequest(GET, url), nonRegistered)).get
      redirection must redirectTo ("/")
      val registeredCosmosSession: CosmosSession = session(redirection)
      registeredCosmosSession must not be 'authenticated
      registeredCosmosSession must not be 'registered
    }

  "A registered user" must "be authenticated and redirected to its profile" in
    new WithTestApplication {
      registerUser(dao, MockOAuthConstants.User101)
      val redirection = oauthRedirectionWithCode(MockOAuthConstants.GrantedCode)
      redirection must redirectTo ("/")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
      cosmosSession must be ('registered)
    }

  "A user rejecting authorization" must "see error information" in new WithTestApplication {
    val result = oauthRedirectionWithError("unauthorized_client")
    status(result) must be (UNAUTHORIZED)
    contentAsString(result) must include ("Access denied")
  }

  "A user with an invalid token" must "see error information and lose her session" in
    new WithTestApplication {
      val result = oauthRedirectionWithCode("invalid")
      val cosmosSession: CosmosSession = session(result)
      cosmosSession must not be 'authenticated
      cosmosSession must not be 'registered
      status(result) must be (UNAUTHORIZED)
      contentAsString(result) must include ("Authorization failed")
    }

  "The / route" must "link to the authorization provider when unauthorized" in
    new WithTestApplication {
      val home = route(FakeRequest(GET, "/")).get
      status(home) must equal (OK)
      val authUrl = authenticationUrl(contentAsString(home))
      authUrl must be ('defined)
    }

  it must "show the registration page when authorized but not registered" in
    new WithTestApplication {
      val session = new Session()
        .setUserProfile(MockOAuthConstants.User101)
        .setToken("token")
      val registrationPage = route(withSession(FakeRequest(GET, "/"), session)).get
      status(registrationPage) must equal (OK)
      contentAsString(registrationPage) must include ("User registration")
    }

  it must "show the user profile when authorized and registered" in new WithTestApplication {
    registerUser(dao, MockOAuthConstants.User101)
    val session = new Session()
      .setUserProfile(MockOAuthConstants.User101)
      .setToken("token")
      .setCosmosId(1)
    val registrationPage = route(withSession(FakeRequest(GET, "/"), session)).get
    status(registrationPage) must equal (OK)
    contentAsString(registrationPage) must include ("Cosmos user profile")
  }

  private def oauthRedirectionWithCode(grantedCode: String) = oauthRedirection(s"code=$grantedCode")

  private def oauthRedirectionWithError(error: String) = oauthRedirection(s"error=$error")

  private def oauthRedirection(queryString: String) =
    route(FakeRequest(GET, s"/auth/${MockOAuthConstants.ProviderId}?$queryString")).get

  private def withSession[A](request: FakeRequest[A], session: Session) =
    request.withSession(session.data.toSeq: _*)

  private def registerUser(dao: CosmosProfileDao, user: UserProfile) {
    dao.withConnection { implicit c =>
      val UserProfile(authId, _, email) = user
      val handle = email.map(_.split('@')(0)).getOrElse("root")
      dao.registerUserInDatabase(authId, Registration(handle, "pk1234"))
    }
  }

  private def authenticationUrl(page: String) =
    """<a class="login" href="(.*?)">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1)))

  private def registrationUrl(page: String) =
    """<form .*action="(.*?)".*>""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1))).get

  private def cancelRegistrationUrl(page: String) =
    """<a href="(.*?)" class="btn cancel">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1))).get
}
