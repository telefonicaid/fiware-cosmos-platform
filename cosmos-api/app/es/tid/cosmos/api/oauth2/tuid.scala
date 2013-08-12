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
import dispatch.{Future => _, _}, Defaults._
import play.api.libs.json.Json

import es.tid.cosmos.api.oauth2.OAuthTupleBuilder._

object TuIdOAuthClient extends OAuthClient {
  override def signUpUrl: String = s"${authorizationUrl.url}/signup/validate/"

  override def authenticateUrl(redirectUri: String): String =
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
      .map(response => ((Json.parse(response) \ "access_token").as[String]))
  }

  override def requestUserProfile(token: String): Future[UserProfile] = {
    val headers: Map[String, String] = Map("Authorization" -> s"Bearer $token")
    Http(apiUrl / "profile" / "me" <:< headers OAuthOK as.String).map(parseTuProfile)
  }

  private def parseTuProfile(str: String): UserProfile = {
    val json = Json.parse(str)
    (json \ "userId").asOpt[String].map(id =>
      UserProfile(
        id=id,
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

  private def authorizationUrl = urlFromConfig("tuid.auth.url")

  private def apiUrl = urlFromConfig("tuid.api.url")
}

trait TuIdOAuthClientComponent extends OAuthClientComponent {
  lazy val oAuthClient: OAuthClient = TuIdOAuthClient
}
