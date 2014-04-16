/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers.pages

import scala.concurrent.Future

import play.api.http.Writeable
import play.api.libs.json.JsValue
import play.api.mvc.Session
import play.api.mvc.SimpleResult
import play.api.test.Helpers._
import play.api.test.FakeRequest

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.api.quota.{Group, NoGroup}

/** A series of user sessions to test with users on different states and roles */
trait WithSampleSessions extends WithTestApplication with WithSampleGroups {

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

  class RegisteredUserSession(val handle: String, name: String, val group: Group = NoGroup) extends UserSession {
    val cosmosProfile = buildCosmosProfile()
    private val groupName = if (group == NoGroup) None else Some(group.name)
    store.withTransaction { implicit c =>
      store.profile.setGroup(cosmosProfile.id, groupName)
    }
    val email = cosmosProfile.email
    val apiCredentials = Some(cosmosProfile.apiCredentials)
    val userProfile = OAuthUserProfile(
      id = CosmosProfileTestHelpers.userIdFor(handle),
      name = Some(name),
      email = Some(email)
    )
    val session = Session().setUserProfile(userProfile).setToken("token")

    def assignCluster(cluster: ClusterId, shared: Boolean) = {
      store.withConnection { implicit c =>
        store.cluster.register(cluster, cosmosProfile.id, ClusterSecret.random(), shared)
      }
    }

    def asClusterUser(sshEnabled: Boolean = true): ClusterUser = ClusterUser(
      username = handle,
      publicKey = cosmosProfile.keys.head.signature,
      sshEnabled = sshEnabled
    )

    protected def buildCosmosProfile(): CosmosProfile =
      CosmosProfileTestHelpers.registerUser(handle)(store)
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

  /** Authenticated and registered users */
  val regUserInGroup = new RegisteredUserSession("reguser1", "User 1", noQuotaGroup)
  val regUserNoGroup = new RegisteredUserSession("reguser2", "User 1")

  def userWithState(state: UserState) =
    new RegisteredUserSession(state.toString, s"${state.toString} 1") {
      override protected def buildCosmosProfile(): CosmosProfile = {
        val profile = super.buildCosmosProfile()
        store.withTransaction { implicit c =>
          store.profile.setUserState(profile.id, state)
          store.profile.lookupByProfileId(profile.id).get
        }
      }
    }

  /** Authenticated system operator */
  val opUser = new RegisteredUserSession("operator", "Mr Operator") {
    override protected def buildCosmosProfile(): CosmosProfile = {
      val profile = super.buildCosmosProfile()
      store.withTransaction { implicit c =>
        store.capability.enable(profile.id, Capability.IsOperator)
        store.profile.lookupByProfileId(profile.id).get
      }
    }
  }
}
