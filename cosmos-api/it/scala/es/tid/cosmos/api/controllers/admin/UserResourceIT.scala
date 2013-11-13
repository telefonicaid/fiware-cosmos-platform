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

package es.tid.cosmos.api.controllers.admin

import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.{HavePropertyMatchResult, HavePropertyMatcher, MustMatchers}
import play.api.libs.json.{Reads, JsValue, JsObject, Json}
import play.api.mvc.SimpleResult
import play.api.test.FakeRequest
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.{MockAuthConstants, WithTestApplication}
import es.tid.cosmos.api.controllers.MaintenanceModeBehaviors
import es.tid.cosmos.api.controllers.common.BasicAuth
import es.tid.cosmos.api.profile.{NamedKey, Registration, UserId}

class UserResourceIT extends FlatSpec with MustMatchers with MaintenanceModeBehaviors {

  val newUserId = UserId(id = "new_id", realm = MockAuthConstants.ProviderId)
  val email = "myhandle@host"
  val publicKey = s"ssh-rsa XXXXXX $email"
  val requestedHandle = "myhandle"
  val resource = "/admin/v1/user"
  val validPayload = Json.obj(
    "authId" -> newUserId.id,
    "authRealm" -> newUserId.realm,
    "handle" -> requestedHandle,
    "email" -> email,
    "sshPublicKey" -> publicKey
  )
  val validAuth = BasicAuth(MockAuthConstants.ProviderId, MockAuthConstants.AdminPassword)

  def post(payload: JsObject, auth: Option[String] = Some(validAuth)): Future[SimpleResult] =
    post(payload.toString, auth)

  def post(payload: String, auth: Option[String]): Future[SimpleResult] = {
    val request = FakeRequest(POST, resource).withBody(payload)
    route(auth.map(a => request.withHeaders("Authorization" -> a)).getOrElse(request)).get
  }

  "The user resource" must behave like resourceDisabledWhenUnderMaintenance(
    FakeRequest(POST, resource).withJsonBody(Json.obj()))

  it must "register a new user when posted" in new WithTestApplication {
    val response = post(validPayload)
    status(response) must be (CREATED)
    val responseData = Json.parse(contentAsString(response))
    responseData must have (field[String]("apiKey"))
    responseData must have (field[String]("apiSecret"))
    (responseData \ "handle").as[String] must be (requestedHandle)
    val createdProfile = dao.withTransaction { implicit c =>
      dao.lookupByUserId(newUserId).getOrElse(fail("User was not created"))
    }
    createdProfile.handle must be (requestedHandle)
    createdProfile.keys must be (Seq(NamedKey("default", publicKey)))
  }

  it must "register a new user with auto generated id" in new WithTestApplication {
    val response = post(validPayload - "handle")
    status(response) must be (CREATED)
    Json.parse(contentAsString(response)) must have (field[String]("handle"))
    val createdProfile = dao.withTransaction { implicit c =>
      dao.lookupByUserId(newUserId).getOrElse(fail("User was not created"))
    }
  }

  it must "reject non authenticated posts" in new WithTestApplication {
    status(post(validPayload, auth = None)) must be (UNAUTHORIZED)
  }

  it must "reject post with invalid credentials" in new WithTestApplication {
    val wrongAuth = BasicAuth("horizon", "wrong password")
    status(post(validPayload, auth = Some(wrongAuth))) must be (FORBIDDEN)
  }

  it must "reject correct credentials for a different realm" in new WithTestApplication {
    val response = post(validPayload ++ Json.obj("authRealm" -> "other_realm"))
    status(response) must be (FORBIDDEN)
    contentAsString(response) must include ("Cannot register users")
  }

  it must "reject requests with malformed JSON" in new WithTestApplication {
    val response = post("not a json", Some(validAuth))
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("Invalid Json")
  }

  it must "reject requests when handle is already taken" in new WithTestApplication {
    dao.withTransaction { implicit c =>
      dao.registerUserInDatabase(
        UserId("otherUser"), Registration(requestedHandle, publicKey, email))
    }
    val response = post(validPayload)
    status(response) must be (CONFLICT)
    contentAsString(response) must include (s"Handle '$requestedHandle' is already taken")
  }

  it must "reject requests when credentials are already registered" in new WithTestApplication {
    dao.withTransaction { implicit c =>
      dao.registerUserInDatabase(newUserId, Registration("otherHandle", publicKey, email))
    }
    val response = post(validPayload)
    status(response) must be (CONFLICT)
    contentAsString(response) must include (s"Already existing credentials: $newUserId")
  }

  def field[T: Reads](fieldName: String) = new HavePropertyMatcher[JsValue, String](){
    def apply(json: JsValue) = HavePropertyMatchResult(
      matches = (json \ fieldName).asOpt[T].isDefined,
      propertyName = fieldName,
      expectedValue = "present",
      actualValue = "missing"
    )
  }
}
