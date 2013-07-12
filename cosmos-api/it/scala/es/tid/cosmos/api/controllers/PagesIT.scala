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

import java.net.URI

import org.apache.commons.lang3.StringEscapeUtils
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc.Session
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo
import es.tid.cosmos.api.controllers.pages.{CosmosSession, Registration}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.oauth2.UserProfile
import es.tid.cosmos.api.mocks.{WithMockedIdentityService, User}
import es.tid.cosmos.api.profile.CosmosProfileDao

class PagesIT extends FlatSpec with MustMatchers {

  "A non registered user" must "be authenticated and get to the registration page" in
    new WithMockedIdentityService {
      val authUrl = mustLinkTheAuthorizationProvider()
      val redirectUrl = identityService.requestAuthorizationCode(
        authUrl, identityService.users.head.id)
      val redirection = route(FakeRequest(GET, relativePath(redirectUrl))).get
      redirection must redirectTo ("/")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
      cosmosSession must not be ('registered)
    }

  it must "be registered upon adding the necessary registration information" in
    new WithMockedIdentityService {
      val authUrl = mustLinkTheAuthorizationProvider()
      val redirectUrl = identityService.requestAuthorizationCode(
        authUrl, identityService.users.head.id)
      val redirection = route(FakeRequest(GET, relativePath(redirectUrl))).get
      redirection must redirectTo ("/")
      val resultSession = session(redirection)
      val cosmosSession: CosmosSession = resultSession
      cosmosSession must be ('authenticated)
      cosmosSession must not be ('registered)
      val registrationPage = route(FakeRequest(GET, "/").withSession(resultSession.data.toSeq: _*)).get
      val regRedirection = route(FakeRequest(POST, registrationUrl(contentAsString(registrationPage)))
        .withFormUrlEncodedBody("handle" -> "user1", "publicKey" -> "SSH_PUBLIC_KEY")
        .withSession(resultSession.data.toSeq: _*)).get
      regRedirection must redirectTo ("/")
      val registeredCosmosSession: CosmosSession = session(regRedirection)
      registeredCosmosSession must be ('authenticated)
      registeredCosmosSession must be ('registered)
    }

  it must "not be authenticated after cancelling the registration process" in
    new WithMockedIdentityService {
      val authUrl = mustLinkTheAuthorizationProvider()
      val redirectUrl = identityService.requestAuthorizationCode(
        authUrl, identityService.users.head.id)
      val redirection = route(FakeRequest(GET, relativePath(redirectUrl))).get
      redirection must redirectTo ("/")
      val resultSession = session(redirection)
      val cosmosSession: CosmosSession = resultSession
      cosmosSession must be ('authenticated)
      cosmosSession must not be ('registered)
      val registrationPage = route(FakeRequest(GET, "/").withSession(resultSession.data.toSeq: _*)).get
      val cancelRegistrationRedirection = route(
        FakeRequest(GET, cancelRegistrationUrl(contentAsString(registrationPage)))
        .withSession(resultSession.data.toSeq: _*)).get
      cancelRegistrationRedirection must redirectTo ("/")
      val registeredCosmosSession: CosmosSession = session(cancelRegistrationRedirection)
      registeredCosmosSession must not be ('authenticated)
      registeredCosmosSession must not be ('registered)
    }

  "A registered user" must "be authenticated and redirected to its profile" in
    new WithMockedIdentityService {
      registerUsers(dao, identityService.users)
      val authUrl = mustLinkTheAuthorizationProvider()
      val redirectUrl = identityService.requestAuthorizationCode(
        authUrl, identityService.users.head.id)
      val redirection = route(FakeRequest(GET, relativePath(redirectUrl))).get
      redirection must redirectTo ("/")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
      cosmosSession must be ('registered)
    }

  "A user rejecting authorization" must "see error information" in new WithMockedIdentityService {
    val authUrl = mustLinkTheAuthorizationProvider()
    val redirectUrl = identityService.requestAuthorizationCode(
      authUrl, identityService.users.head.id, isAuthorized = false)
    val result = route(FakeRequest(GET, relativePath(redirectUrl))).get
    status(result) must be (UNAUTHORIZED)
    contentAsString(result) must include ("Access denied")
  }

  "A user with an invalid token" must "see error information and lose her session" in
    new WithMockedIdentityService {
      val result = route(FakeRequest(GET, "/auth?code=invalid")).get
      val cosmosSession: CosmosSession = session(result)
      cosmosSession must not be ('authenticated)
      cosmosSession must not be ('registered)
      status(result) must be (UNAUTHORIZED)
      contentAsString(result) must include ("Authorization failed")
    }

  "The / route" must "link to the authorization provider when unauthorized" in
    new WithMockedIdentityService {
      mustLinkTheAuthorizationProvider()
    }

  it must "show the registration page when authorized but not registered" in
    new WithMockedIdentityService {
      val session = new Session()
        .setUserProfile(UserProfile(identityService.users.head.id))
        .setToken("token")
      val registrationPage = route(FakeRequest(GET, "/").withSession(session.data.toSeq: _*)).get
      status(registrationPage) must equal (OK)
      contentAsString(registrationPage) must include ("User registration")
    }

  it must "show the user profile when authorized and registered" in new WithMockedIdentityService {
    registerUsers(dao, identityService.users)
    val user = identityService.users.head
    val session = new Session()
      .setUserProfile(UserProfile(user.id))
      .setToken("token")
      .setCosmosId(1)
    val registrationPage = route(FakeRequest(GET, "/").withSession(session.data.toSeq: _*)).get
    status(registrationPage) must equal (OK)
    contentAsString(registrationPage) must include ("Cosmos user profile")
  }

  private def mustLinkTheAuthorizationProvider(): String = {
    val home = route(FakeRequest(GET, "/")).get
    status(home) must equal (OK)
    val authUrl = authenticationUrl(contentAsString(home))
    authUrl must be ('defined)
    authUrl.get
  }

  private def registerUsers(dao: CosmosProfileDao, users: Iterable[User]) {
    dao.withConnection { implicit c =>
      for (User(tuId, _, _, email) <- users) {
        val handle = email.map(_.split('@')(0)).getOrElse("root")
        dao.registerUserInDatabase(tuId, Registration(handle, "pk1234"))
      }
    }
  }

  private def authenticationUrl(page: String) =
    """<a id="login" href="(.*?)">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1)))

  private def registrationUrl(page: String) =
    """<form .*action="(.*?)".*>""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1))).get

  private def cancelRegistrationUrl(page: String) =
    """<a href="(.*?)" class="btn cancel">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1))).get

  private def relativePath(url: String) = {
    val uri = URI.create(url)
    s"${uri.getPath}?${uri.getQuery}"
  }
}
