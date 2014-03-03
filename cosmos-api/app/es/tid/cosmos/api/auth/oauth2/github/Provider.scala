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

package es.tid.cosmos.api.auth.oauth2.github

import scala.concurrent.Future

import com.typesafe.config.Config
import dispatch.{Future => _, _}, Defaults._
import play.api.libs.json.{Reads, JsValue, Json}

import es.tid.cosmos.api.auth.oauth2.{OAuthUserProfile, JsonProfileParser, AbstractOAuthProvider}
import es.tid.cosmos.api.auth.oauth2.OAuthTupleBuilder._

/** OAuth2 integration with GitHub */
class Provider(id: String, config: Config) extends AbstractOAuthProvider(id, config) {

  override def authenticationUrl(redirectUri: String): String =
    (authorizationUrl / "authorize" <<? Map(
      "client_id" -> clientId,
      "scope" -> "user",
      "redirect_uri" -> redirectUri
    )).build().getRawUrl

  override def requestAccessToken(code: String, redirectUrl: String): Future[String] = {
    val queryParams = Map(
      "client_id" -> clientId,
      "client_secret" -> clientSecret,
      "code" -> code
    )
    val headers = Map(
      "Accept" -> "application/json"
    )
    val request = authorizationUrl / "access_token" <<? queryParams <:< headers << ""
    Http(request OAuthOK as.String)
      .map(response => (Json.parse(response) \ "access_token").as[String]
    )
  }

  override protected val profileParser = new JsonProfileParser(new Reads[OAuthUserProfile]{
    override def reads(json: JsValue) = Json.fromJson[GitHubProfile](json).map(_.asUserProfile(id))
  })

  override protected def requestProfileResource(token: String): Future[String] = {
    val request = apiUrl / "user" <<? Map(
      "access_token" -> token
    )
    Http(request OAuthOK as.String)
  }
}
