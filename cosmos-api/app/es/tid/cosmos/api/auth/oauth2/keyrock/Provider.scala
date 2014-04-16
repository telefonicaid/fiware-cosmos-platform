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

package es.tid.cosmos.api.auth.oauth2.keyrock

import scala.concurrent.Future

import com.typesafe.config.Config
import dispatch.{Future => _, _}, Defaults._
import play.api.libs.json.{Reads, JsValue, Json}

import es.tid.cosmos.api.auth.oauth2.{OAuthUserProfile, JsonProfileParser, AbstractOAuthProvider}
import es.tid.cosmos.api.auth.oauth2.OAuthTupleBuilder._

/** OAuth2 integration with Keyrock */
class Provider(id: String, config: Config) extends AbstractOAuthProvider(id, config) {

  override def authenticationUrl(redirectUrl: String): String =
    (authorizationUrl / "authorize" <<? Map(
      "response_type" -> "code",
      "client_id" -> clientId,
      "redirect_uri" -> redirectUrl
    )).build().getRawUrl

  override def requestAccessToken(code: String, redirectUrl: String): Future[String] = {
    val queryParams = Map(
      "grant_type" -> "authorization_code",
      "code" -> code,
      "redirect_uri" -> redirectUrl
    )
    val headers = Map("Accept" -> "application/json")
    val request = authorizationUrl / "token" <<? queryParams <:< headers << ""
    Http(request.as_!(clientId, clientSecret) OAuthOK as.String)
      .map(response => (Json.parse(response) \ "access_token").as[String])
  }

  override protected val profileParser = new JsonProfileParser(new Reads[OAuthUserProfile]{
    override def reads(json: JsValue) = Json.fromJson[KeyrockProfile](json).map(_.asUserProfile(id))
  })

  /** Request the profile resource contents. */
  override protected def requestProfileResource(token: String): Future[String] = {
    val request = apiUrl / "user" <<? Map("access_token" -> token)
    Http(request OAuthOK as.String)
  }
}
