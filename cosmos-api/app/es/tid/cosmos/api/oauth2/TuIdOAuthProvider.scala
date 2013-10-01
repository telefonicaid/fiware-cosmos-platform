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

package es.tid.cosmos.api.oauth2

import scala.concurrent.Future

import com.ning.http.client.RequestBuilder
import com.typesafe.config.Config
import dispatch.{Future => _, _}, Defaults._
import play.api.libs.json.Json

import es.tid.cosmos.api.oauth2.OAuthTupleBuilder._
import es.tid.cosmos.api.profile.UserId

class TuIdOAuthProvider(id: String, config: Config) extends AbstractOAuthProvider(id, config) {

  override def authenticationUrl(redirectUri: String): String =
    (authorizationUrl / "authorize" <<? Map(
      "client_id" -> clientId,
      "scope" -> "userdata.user.read.basic userdata.user.read.emails",
      "response_type" -> "code",
      "redirect_uri" -> redirectUri
    )).build().getRawUrl

  override def requestAccessToken(code: String): Future[String] = {
    val tokenUrl: RequestBuilder = apiUrl / "oauth2" / "token"
    val queryParams = Map(
      "grant_type" -> "authorization_code",
      "code" -> code
    )
    Http((tokenUrl as_!(clientId, clientSecret)) <<? queryParams << "" OAuthOK as.String)
      .map(response => (Json.parse(response) \ "access_token").as[String])
  }

  override def requestUserProfile(token: String): Future[OAuthUserProfile] = {
    val headers: Map[String, String] = Map("Authorization" -> s"Bearer $token")
    Http(apiUrl / "profile" / "me" <:< headers OAuthOK as.String).map(parseTuProfile)
  }

  private def parseTuProfile(str: String): OAuthUserProfile = {
    val json = Json.parse(str)
    (json \ "userId").asOpt[String].map(userId =>
      OAuthUserProfile(
        id=UserId(id, userId),
        name=makeFullName(
          (json \ "firstName").asOpt[String],
          (json \ "surname").asOpt[String]
        ),
        email=(for (
          addressNode <- json \\ "address";
          address <- addressNode.asOpt[String]
        ) yield address).headOption
      ))
    .getOrElse(throw new IllegalArgumentException("Missing 'userId' field in TU|Profile"))
  }

  private def makeFullName(first: Option[String], last: Option[String]) = {
    val presentParts = Seq(first, last).flatten
    if (presentParts.isEmpty) None
    else Some(presentParts.mkString(" "))
  }
}
