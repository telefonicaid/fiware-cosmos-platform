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

import java.util.NoSuchElementException
import scala.concurrent.Future

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}, Defaults._
import play.api.libs.json.Json
import play.api.Play._

import es.tid.cosmos.api.oauth2.OAuthTupleBuilder._

class TuIdOAuthClient extends OAuthClient {
  override def signUpUrl: String = s"${authorizationUrl.url}/signup/validate/"

  override def authenticateUrl(redirectUri: String): String =
    (authorizationUrl / "authorize" <<? Map(
      "client_id" -> stringConfig("tuid.client.id"),
      "scope" -> "userdata.user.read.basic userdata.user.read.emails",
      "response_type" -> "code",
      "redirect_uri" -> redirectUri
    )).build().getRawUrl

  override def requestAccessToken(code: String): Future[String] = {
    val tokenUrl: RequestBuilder = apiUrl / "oauth2" / "token"
    val clientId = stringConfig("tuid.client.id")
    val secret = stringConfig("tuid.client.secret")
    val queryParams = Map(
      "grant_type" -> "authorization_code",
      "code" -> code
    )
    Http((tokenUrl as_!(clientId, secret)) <<? queryParams << "" OAuthOK as.String)
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
        firstName=(json \ "firstName").asOpt[String],
        surname=(json \ "surname").asOpt[String],
        email=(for (addressNode <- (json \\ "address");
                   address <- addressNode.asOpt[String])
                yield address).headOption
      ))
    .getOrElse(throw new IllegalArgumentException("Missing 'userId' field in TU|Profile"))
  }

  private def authorizationUrl = urlFromConfig("tuid.auth.url")

  private def apiUrl = urlFromConfig("tuid.api.url")

  private def urlFromConfig(key: String) = url(stringConfig(key))

  private def stringConfig(key: String) = try {
    current.configuration.getString(key).get
  } catch {
    case ex: NoSuchElementException =>
      throw new IllegalArgumentException(s"Missing required configuration key $key", ex)
  }
}

trait TuIdOAuthClientComponent extends OAuthClientComponent {
  lazy val oAuthClient: OAuthClient = new TuIdOAuthClient
}
