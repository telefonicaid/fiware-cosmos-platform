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
import play.api.http.Writeable
import play.api.libs.json.{Reads, JsValue, JsObject, Json}
import play.api.mvc.SimpleResult
import play.api.test.FakeRequest
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.{MockAuthConstants, WithTestApplication}
import es.tid.cosmos.api.controllers.MaintenanceModeBehaviors
import es.tid.cosmos.api.controllers.common.BasicAuth
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Registration

class UserResourceIT extends FlatSpec with MustMatchers with MaintenanceModeBehaviors {

  val newUserId = UserId(id = "new_id", realm = MockAuthConstants.ProviderId)
  val email = "myhandle@host"
  val publicKey = s"ssh-rsa XXXXXX $email"
  val requestedHandle = "myhandle"
  val userResource = "/admin/v1/user"
  val validPayload = Json.obj(
    "authId" -> newUserId.id,
    "authRealm" -> newUserId.realm,
    "handle" -> requestedHandle,
    "email" -> email,
    "sshPublicKey" -> publicKey
  )
  val validAuth = BasicAuth(MockAuthConstants.ProviderId, MockAuthConstants.AdminPassword)

  def postRegistration(payload: JsObject, auth: Option[String] = Some(validAuth)): Future[SimpleResult] =
    postRegistration(payload.toString, auth)

  def postRegistration(payload: String, auth: Option[String]): Future[SimpleResult] =
    authenticatedWith(FakeRequest(POST, userResource).withBody(payload), auth)

  def deleteUser(userId: UserId, auth: Option[String] = Some(validAuth)): Future[SimpleResult] =
    authenticatedWith(FakeRequest(DELETE, s"$userResource/${userId.realm}/${userId.id}"), auth)

  def authenticatedWith[A: Writeable](request: FakeRequest[A], auth: Option[String]) =
    route(auth.map(a => request.withHeaders("Authorization" -> a)).getOrElse(request)).get

  "User registration" must behave like resourceDisabledWhenUnderMaintenance(
    FakeRequest(POST, userResource).withJsonBody(Json.obj()))

  "User unregistration" must behave like resourceDisabledWhenUnderMaintenance(
    FakeRequest(DELETE, s"$userResource/realm/id").withJsonBody(Json.obj()))

  "The user resource" must "register a new user when posted" in new WithTestApplication {
    withPersistentHdfsDeployed {
      val response = postRegistration(validPayload)
      status(response) must be (CREATED)
      val responseData = Json.parse(contentAsString(response))
      responseData must have (field[String]("apiKey"))
      responseData must have (field[String]("apiSecret"))
      (responseData \ "handle").as[String] must be (requestedHandle)
      val createdProfile = store.withTransaction { implicit c =>
        store.profile.lookupByUserId(newUserId).getOrElse(fail("User was not created"))
      }
      createdProfile.handle must be (requestedHandle)
      createdProfile.keys must be (Seq(NamedKey("default", publicKey)))
    }
  }

  it must "register a new user with auto generated id" in new WithTestApplication {
    withPersistentHdfsDeployed {
      val response = postRegistration(validPayload - "handle")
      status(response) must be (CREATED)
      Json.parse(contentAsString(response)) must have (field[String]("handle"))
      store.withTransaction { implicit c =>
        store.profile.lookupByUserId(newUserId).getOrElse(fail("User was not created"))
      }
    }
  }

  it must "reject non authenticated posts" in new WithTestApplication {
    status(postRegistration(validPayload, auth = None)) must be (UNAUTHORIZED)
  }

  it must "reject post with invalid credentials" in new WithTestApplication {
    val wrongAuth = BasicAuth("horizon", "wrong password")
    status(postRegistration(validPayload, auth = Some(wrongAuth))) must be (FORBIDDEN)
  }

  it must "reject requests to unknown realms" in new WithTestApplication {
    val response = postRegistration(validPayload ++ Json.obj("authRealm" -> "other_realm"))
    status(response) must be (NOT_FOUND)
    contentAsString(response) must include ("other_realm authentication realm does not exist")
  }

  it must "reject requests with malformed JSON" in new WithTestApplication {
    val response = postRegistration("not a json", Some(validAuth))
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("Invalid Json")
  }

  it must "reject requests when handle is already taken" in new WithTestApplication {
    store.withTransaction { implicit c =>
      store.profile.register(
        UserId("otherUser"), Registration(requestedHandle, publicKey, email), UserState.Enabled
      )
    }
    val response = postRegistration(validPayload)
    status(response) must be (CONFLICT)
    contentAsString(response) must include (s"Handle '$requestedHandle' is already taken")
  }

  it must "reject requests when credentials are already registered" in new WithTestApplication {
    store.withTransaction { implicit c =>
      store.profile.register(newUserId, Registration("otherHandle", publicKey, email), UserState.Enabled)
    }
    val response = postRegistration(validPayload)
    status(response) must be (CONFLICT)
    contentAsString(response) must include (s"Already existing credentials: $newUserId")
  }

  it must "reject not authenticated unregister requests" in new WithTestApplication {
    status(deleteUser(newUserId, auth = None)) must be (UNAUTHORIZED)
  }

  it must "reject unregistration of users of other realms" in new WithTestApplication {
    status(deleteUser(newUserId.copy(realm = "other_realm"), auth = None)) must be (NOT_FOUND)
  }

  it must "produce 404 when the user does not exist" in new WithTestApplication {
    status(deleteUser(newUserId)) must be (NOT_FOUND)
  }

  it must "unregister existing users of the same realm" in new WithTestApplication {
    withPersistentHdfsDeployed {
      registerUser(MockAuthConstants.User101.copy(id = newUserId))
      val response = deleteUser(newUserId)
      status(response) must be (OK)
      contentAsString(response) must include ("User new_id@id_service unregistration started")
      store.withTransaction { implicit c =>
        store.profile.lookupByUserId(newUserId).get.state
      } must (be (UserState.Deleting) or be (UserState.Deleted))
    }
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
