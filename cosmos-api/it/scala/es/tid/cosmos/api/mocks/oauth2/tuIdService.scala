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

package es.tid.cosmos.api.mocks.oauth2

import scala.util.Random

import com.ning.http.util.Base64
import play.api.libs.json.Json
import unfiltered.jetty.Http
import unfiltered.filter.Planify
import unfiltered.request._
import unfiltered.response._
import unfiltered.response.ResponseString

import es.tid.cosmos.api.mocks.oauth2.UrlUtils.parseQueryParams
import es.tid.cosmos.api.oauth2.UserProfile
import es.tid.cosmos.api.profile.UserId

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
class TuIdService(
    port: Int,
    clientId: String,
    clientSecret: String,
    val users: List[UserProfile] = List(UserProfile(
      id = UserId("tuid", "db001"),
      name= Some("John Smith"),
      email = Some("jsmith@tid.es")
    )),
    val validScopes: Set[String] = Set("userdata.user.read.basic", "userdata.user.read.emails")
  ) {

  @volatile
  private var tokens: Map[String, (String, Seq[String])] = Map()
  @volatile
  private var authorizations: List[Authorization] = List()

  private object GrantType extends Params.Extract("grant_type", Params.first)

  private object Code extends Params.Extract("code", Params.first)

  private val server = Http.local(port).filter(Planify({
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
            user <- users.find(_.id.id == userId))
        yield {
          val basicFields =
            if (scopes.contains("userdata.user.read.basic")) {
              val nameParts = user.name.get.split(" ")
              Json.obj(
                "userId" -> user.id.id,
                "firstName" -> nameParts(0),
                "surname" -> nameParts(1)
              )
            } else Json.obj()
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
   * @return             An authorization code
   */
  def requestAuthorizationCode(url: String, userId: UserId): String = {
    val params = parseQueryParams(url)
    val scopes = params.collectFirst({
      case ("scope", value) => value.split(' ')
    })
    require(scopes.isDefined, "No scopes were requested")
    require(params.contains(("client_id", clientId)), "No valid client id was provided")
    require(params.contains(("response_type", "code")), "Incorrect type of response")
    val redirectUri = params.collectFirst({
      case ("redirect_uri", uri) => uri
    })
    require(redirectUri.isDefined, "No redirection was specified")
    val token = randomToken("auth")
    authorizations = authorizations :+ Authorization(token, userId.id, scopes.get)
    token
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
