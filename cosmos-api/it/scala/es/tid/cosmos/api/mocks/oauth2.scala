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

package es.tid.cosmos.api.mocks

import scala.util.Random

import com.ning.http.util.Base64
import play.api.libs.json.{JsObject, Json}
import unfiltered.jetty.Http
import unfiltered.filter.Planify
import unfiltered.request._
import unfiltered.response._
import unfiltered.response.ResponseString
import java.net.{URLDecoder, URI}

case class User(
    id: String,
    firstName: Option[String],
    surname: Option[String],
    email: Option[String])

object User {
  def apply(id: String, firstName: String, surname: String, email: String): User =
    User(id, Some(firstName), Some(surname), Some(email))
}

case class Authorization(token: String, userId: String, scopes: Seq[String])

/**
 * Mock Oauth2 provider with methods to use it as a test fixture.
 */
class IdentityService(
    val clientId: String = "client123",
    val clientSecret: String = "secret123",
    val users: List[User] = List(User(
      id = "db001",
      firstName= "John",
      surname = "Smith",
      email = "jsmith@tid.es"
    )),
    val validScopes: Set[String] = Set("userdata.user.read.basic", "userdata.user.read.emails")
  ) {
  @volatile
  private var tokens: Map[String, (String, Seq[String])] = Map()
  @volatile
  private var authorizations: List[Authorization] = List()

  private object GrantType extends Params.Extract("grant_type", Params.first)

  private object Code extends Params.Extract("code", Params.first)

  private val server = Http.anylocal.filter(Planify({
    case req @ POST(Path("/oauth2/token") & Params(GrantType(gt) & Code(code))) => {
      require(gt == "authorization_code")
      val authBytes: Array[Byte] = req.headers("Authorization").collectFirst {
        case value: String => Base64.decode(value.substring(6))
      }.get
      require(new String(authBytes) == s"$clientId:$clientSecret")
      (for (Authorization(_, userId, scopes) <- authorizations.find(_.token == code))
        yield {
          val newToken = randomToken("access")
          tokens = tokens.updated(newToken, (userId, scopes))
          ResponseString(Json.obj("access_token" -> newToken).toString)
        }
      ).getOrElse(BadRequest ~> ResponseString("error=invalid_request"))
    }

    case req @ GET(Path("/profile/me")) => {
      (for (accessToken <- bearerToken(req);
            (userId, scopes) <- tokens.get(accessToken);
            user <- users.find(_.id == userId))
        yield {
          val basicFields =
            if (scopes.contains("userdata.user.read.basic")) Json.obj(
              "userId" -> user.id,
              "firstName" -> user.firstName,
              "surname" -> user.surname
            )
            else Json.obj()
          val emailField =
            if (scopes.contains("userdata.user.read.emails")) Json.obj("address" -> user.email)
            else Json.obj()
        ResponseString((basicFields ++ emailField).toString)
      }).getOrElse(Unauthorized)
    }
  }))

  def baseUrl: String = server.url
  def issuedTokens: Iterable[String] = tokens.keys

  /**
   * Simulates an authorization code request.
   *
   * @param url          Authorization request url
   * @param userId       User that is going to accept the request
   * @param isAuthorized Whether the user is granting access rights
   * @return             URL the identity provider would have redirected
   */
  def requestAuthorizationCode(
      url: String,
      userId: String,
      isAuthorized: Boolean = true): String = {
    val pairs = url.substring(url.indexOf('?') + 1).split('&')
      .map(parameter => parameter.split('=') match {
        case Array(key, value) => (key, value)
      })
    val scopes = pairs.collectFirst({
      case ("scope", value) => value.split(' ')
    })
    require(scopes.isDefined, "No scopes were requested")
    require(pairs.contains(("client_id", clientId)), "No valid client id was provided")
    require(pairs.contains(("response_type", "code")), "Incorrect type of response")
    val redirectUri = pairs.collectFirst({
      case ("redirect_uri", uri) => uri
    })
    require(redirectUri.isDefined, "No redirection was specified")
    if (isAuthorized) {
      val token = randomToken("auth")
      authorizations = authorizations :+ Authorization(token, userId, scopes.get)
      s"${redirectUri.get}?code=$token"
    } else {
      s"${redirectUri.get}?error=unauthorized_client"
    }
  }

  def start() {
    server.start()
  }

  def stop() {
    server.stop()
  }

  private def bearerToken(req: HttpRequest[Any]) =
    req.headers("Authorization").toList match {
      case value :: _ => Some(value.substring(7))
      case _ => None
    }

  private def randomToken(prefix: String) = s"$prefix-${Random.nextLong().abs}"
}
