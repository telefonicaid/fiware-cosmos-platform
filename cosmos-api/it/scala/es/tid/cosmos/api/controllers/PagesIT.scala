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

import scala.language.reflectiveCalls

import org.apache.commons.lang3.StringEscapeUtils
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo
import es.tid.cosmos.api.controllers.pages.{WithSampleSessions, CosmosSession}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.{MockAuthConstants, WithTestApplication}

class PagesIT extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  "The index page" must "show the landing page with auth links for unauthorized users" in
    new WithSampleSessions {
      val landingPage = unauthUser.doRequest("/")
      status(landingPage) must equal (OK)
      contentAsString(landingPage) must include ("id=\"auth-selector\"")
      authenticationUrl(contentAsString(landingPage)) must be ('defined)
    }

  it must behave like pageDisabledWhenUnderMaintenance(FakeRequest(GET, "/"))

  it must "redirect to the registration form for the unregistered users" in new WithSampleSessions {
    unregUser.doRequest("/") must redirectTo ("/register")
  }

  it must "redirect to the user profile page for registered users" in new WithSampleSessions {
    regUser.doRequest("/") must redirectTo ("/profile")
  }

  "The registration page" must "redirect to the index unauthenticated users" in
    new WithSampleSessions {
      unauthUser.doRequest("/register") must redirectTo ("/")
    }

  it must behave like pageDisabledWhenUnderMaintenance(FakeRequest(GET, "/register"))
  it must behave like resourceDisabledWhenUnderMaintenance(FakeRequest(POST, "/register"))

  it must "show the registration page to the unregistered users" in new WithSampleSessions {
    val registrationPage = unregUser.doRequest("/register")
    status(registrationPage) must equal (OK)
    contentAsString(registrationPage) must include ("User registration")
  }

  it must "log out users after cancelling the registration process" in new WithSampleSessions {
    val registrationPage = unregUser.doRequest("/register")
    val url = cancelRegistrationUrl(contentAsString(registrationPage))
    val response = unregUser.doRequest(url)
    response must redirectTo ("/")
    val responseSession: CosmosSession = session(response)
    responseSession must not be 'authenticated
  }

  it must "redirect to the index registered users" in new WithSampleSessions {
    regUser.doRequest("/register") must redirectTo ("/")
  }

  it must "register unregistered users" in new WithSampleSessions {
    val response = unregUser.submitForm("/register",
      "handle" -> "newuser",
      "publicKey" -> "ssh-rsa DKDJDJDK jsmith@example.com")
    val contents = contentAsString(response)
    response must redirectTo ("/")
    dao.withConnection { implicit c =>
      dao.lookupByUserId(unregUser.userId) must be ('defined)
    }
  }

  it must "reject registrations when the submitted form is invalid" in new WithSampleSessions {
    val response = unregUser.submitForm("/register",
      "handle" -> "1nvalid handle",
      "publicKey" -> "ssh-rsa DKDJDJDK jsmith@example@invalid.com"
    )
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("Not a valid unix handle")
    contentAsString(response) must include ("invalid email")
  }

  it must "reject registrations when selected handle is already taken" in new WithSampleSessions {
    val response = unregUser.submitForm("/register",
      "handle" -> regUser.handle,
      "publicKey" -> "ssh-rsa DKDJDJDK jsmith@example.com"
    )
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("already taken")
  }

  "The profile page" must "show the user profile page for registered users" in new WithSampleSessions {
    val profilePage = regUser.doRequest("/profile")
    status(profilePage) must equal (OK)
    contentAsString(profilePage) must include (s"Profile for ${regUser.userProfile.contact}")
  }

  it must behave like pageForRegistreredUsers("/profile")
  it must behave like pageDisabledWhenUnderMaintenance(FakeRequest(GET, "/profile"))

  "The getting started page" must "show a personalized getting started tutorial" in
    new WithSampleSessions {
      val response = regUser.doRequest("/getting-started")
      contentAsString(response) must (
        include (regUser.cosmosProfile.apiCredentials.apiKey) and
        include (regUser.cosmosProfile.apiCredentials.apiSecret))
    }

  it must behave like pageForRegistreredUsers("/getting-started")
  it must behave like pageDisabledWhenUnderMaintenance(FakeRequest(GET, "/getting-started"))

  "The OAuth authorization resource" must behave like
    pageDisabledWhenUnderMaintenance(FakeRequest(GET, "/auth/provider"))

  "A registered user" must "be authenticated after OAuth redirection" in
    new WithTestApplication {
      registerUser(dao, MockAuthConstants.User101)
      val redirection = oauthRedirectionWithCode(MockAuthConstants.GrantedCode)
      redirection must redirectTo ("/profile")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
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
      status(result) must be (UNAUTHORIZED)
      contentAsString(result) must include ("Authorization failed")
    }

  private def oauthRedirectionWithCode(grantedCode: String) = oauthRedirection(s"code=$grantedCode")

  private def oauthRedirectionWithError(error: String) = oauthRedirection(s"error=$error")

  private def oauthRedirection(queryString: String) =
    route(FakeRequest(GET, s"/auth/${MockAuthConstants.ProviderId}?$queryString")).get

  private def authenticationUrl(page: String) =
    """<a class="login" href="(.*?)">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1)))

  private def cancelRegistrationUrl(page: String) =
    """<a href="(.*?)" class="btn cancel">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1))).get
}
