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

package es.tid.cosmos.api.controllers.pages

import scala.Some
import scala.concurrent.Future

import play.api.http.Writeable
import play.api.libs.json.JsValue
import play.api.mvc.Session
import play.api.mvc.SimpleResult
import play.api.test.Helpers._
import play.api.test.FakeRequest

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.servicemanager.ClusterUser

/** A series of user sessions to test with users on different states and roles */
trait WithSampleSessions extends WithTestApplication {

  /** Represents a user session */
  trait UserSession {
    val session: Session
    val apiCredentials: Option[ApiCredentials]

    def request(path: String, method: String = GET) =
      FakeRequest(method, path).withSession(session.data.toSeq: _*)

    def doRequest(path: String, method: String = GET): Future[SimpleResult] =
      route(request(path, method)).get

    def doRequest[T: Writeable](request: FakeRequest[T]): Future[SimpleResult] =
      route(request.withSession(session.data.toSeq: _*)).get

    def submitForm(path: String, fields: (String, String)*): Future[SimpleResult] =
      route(request(path, POST).withFormUrlEncodedBody(fields: _*)).get

    def submitJson(path: String, body: JsValue, method: String = POST): Future[SimpleResult] =
      route(request(path, method).withJsonBody(body)).get
  }

  class RegisteredUserSession(val handle: String, name: String) extends UserSession {
    val cosmosProfile = buildCosmosProfile()
    val email = cosmosProfile.email
    val apiCredentials = Some(cosmosProfile.apiCredentials)
    val userProfile = OAuthUserProfile(
      id = CosmosProfileTestHelpers.userIdFor(handle),
      name = Some(name),
      email = Some(email)
    )
    val session = Session().setUserProfile(userProfile).setToken("token")

    def setAsOwner(cluster: ClusterId) = {
      dao.withConnection { implicit c =>
        dao.assignCluster(cluster, cosmosProfile.id)
      }
    }

    def asClusterUser: ClusterUser = ClusterUser(
      username = handle,
      publicKey = cosmosProfile.keys.head.signature
    )

    protected def buildCosmosProfile(): CosmosProfile =
      CosmosProfileTestHelpers.registerUser(handle)(dao)
  }

  /** Not authenticated user */
  val unauthUser = new UserSession {
    val session = Session()
    val apiCredentials = None
  }

  /** User authenticated but not registered */
  val unregUser = new UserSession {
    val userId = UserId("unreg")
    val userProfile = OAuthUserProfile(
      id = userId,
      name = Some("unregistered"),
      email = Some("unreg@mail.com")
    )
    val session = Session().setUserProfile(userProfile).setToken("token")
    val apiCredentials = None
  }

  /** Authenticated and registered user */
  val regUser = new RegisteredUserSession("reguser", "User 1")

  def userWithState(state: UserState) =
    new RegisteredUserSession(state.toString, s"${state.toString} 1") {
      override protected def buildCosmosProfile(): CosmosProfile = {
        val profile = super.buildCosmosProfile()
        dao.withTransaction { implicit c =>
          dao.setUserState(profile.id, state)
        }
        profile
      }
    }

  /** Authenticated system operator */
  val opUser = new RegisteredUserSession("operator", "Mr Operator") {
    override protected def buildCosmosProfile(): CosmosProfile = {
      val profile = super.buildCosmosProfile()
      dao.withTransaction { implicit c =>
        dao.enableUserCapability(profile.id, Capability.IsOperator)
      }
      profile
    }
  }
}
