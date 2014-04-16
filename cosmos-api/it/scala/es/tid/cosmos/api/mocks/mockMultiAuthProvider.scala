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

package es.tid.cosmos.api.mocks

import scala.concurrent.Future

import es.tid.cosmos.api.auth.oauth2._
import es.tid.cosmos.api.auth.multiauth.{MultiAuthProviderComponent, MultiAuthProvider}
import es.tid.cosmos.api.profile.UserId

object MockAuthConstants {
  val BaseUrl = "http://mock-oauth"
  val GrantedCode = "fake-code"
  val GrantedToken = "fake-token-123"
  val User101 = OAuthUserProfile(UserId("101"), Some("John Smith"), Some("jsmith@tid.es"))
  val ProviderId = "id_service"
  val AdminPassword = "sample password"
}

object MockAuthProvider extends OAuthProvider {
  import MockAuthConstants._

  override val id = ProviderId

  override val name = ProviderId.replace("_", " ")

  override def newAccountUrl: Option[String] = Some(s"$BaseUrl/signup/")

  override def authenticationUrl(redirectUri: String): String =
    s"$BaseUrl/oauth?client_id=fake&redirect_to=$redirectUri"

  override def requestAccessToken(code: String, redirectUrl: String): Future[String] =
    if (code == GrantedCode) Future.successful(GrantedToken)
    else Future.failed(OAuthException(OAuthError.InvalidGrant, "testing invalid grant"))

  override def requestUserProfile(token: String): Future[OAuthUserProfile] =
    if (token == GrantedToken) Future.successful(User101)
    else Future.failed(OAuthException(OAuthError.InvalidRequest, "testing invalid requests"))

  override val adminPassword = Some(AdminPassword)
}

object MockMultiAuthProvider extends MultiAuthProvider {
  override val providers = Map(MockAuthConstants.ProviderId -> MockAuthProvider)
  override val tokenAuthenticationProvider = None
}

trait MockMultiAuthProviderComponent extends MultiAuthProviderComponent {
  override val multiAuthProvider: MultiAuthProvider = MockMultiAuthProvider
}
