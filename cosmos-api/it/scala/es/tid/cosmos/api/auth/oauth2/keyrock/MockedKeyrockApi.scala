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

package es.tid.cosmos.api.auth.oauth2.keyrock

import scala.concurrent.stm.{TSet, atomic}
import scala.util.Random

import play.api.libs.json.Json
import unfiltered.filter.Planify
import unfiltered.jetty.Http
import unfiltered.request._
import unfiltered.response._

import es.tid.cosmos.api.auth.oauth2.github.UrlUtils.parseQueryParams

class MockedKeyrockApi(
    port: Int,
    clientId: String,
    clientSecret: String,
    redirectUri: String,
    existingUser: KeyrockProfile
  ) {

  private val grantedCodes = TSet.empty[String]
  private val grantedTokens = TSet.empty[String]
  private val server = defineServer()

  def requestAuthorizationCode(authUrl: String, userId: Int): String = {
    val params = parseQueryParams(authUrl)
    require(params.contains("client_id" -> clientId), "Missing client id in code request")
    require(params.contains("response_type" -> "code"),
      "Response type is missing or different from 'code'")
    require(params.exists(pair => pair._1 == "redirect_uri"))
    val code = randomId()
    atomic { implicit txn =>
      grantedCodes.add(code)
    }
    code
  }

  def start() {
    server.start()
  }

  def stop() {
    server.stop()
  }

  def clear() {
    atomic { implicit txt =>
      grantedCodes.clear()
      grantedTokens.clear()
    }
  }

  def configurationKeys: Map[String, String] = {
    val baseUrl = s"http://127.0.0.1:$port/"
    Map(
      "name" -> "Keyrock",
      "class" -> classOf[Provider].getName,
      "client.id" -> clientId,
      "client.secret" -> clientSecret,
      "auth.url" -> baseUrl,
      "api.url" -> baseUrl,
      "signup.url" -> s"${baseUrl}signup"
    )
  }

  private def defineServer() = {
    object GrantType extends Params.Extract("grant_type", Params.first)
    object Code extends Params.Extract("code", Params.first)
    object Token extends Params.Extract("access_token", Params.first)
    object RedirectUri extends Params.Extract("redirect_uri", Params.first)

    Http.local(port).filter(Planify({
      case req @ POST(Path("/token") &
        Params(GrantType(grantType) & Code(code) & RedirectUri(uriParam))) & BasicAuth(id, password) =>
        require(grantType == "authorization_code",
          "Grant type missing or different to 'authorization_code'")
        require(id == clientId, s"Token request was not requested by $clientId")
        require(password == clientSecret, s"Token request didn't used the right password")
        atomic { implicit txn =>
          if (redirectUri == uriParam && grantedCodes.contains(code)) {
            val token = randomId()
            grantedTokens.add(token)
            ResponseString(Json.obj("access_token" -> token).toString())
          } else BadRequest ~> ResponseString("error=invalid_request")
        }

      case req @ GET(Path("/user") & Params(Token(token))) => atomic { implicit txn =>
        if (grantedTokens.contains(token)) ResponseString(
          s"""{
           |  "id": ${existingUser.id},
           |  "actorId": ${existingUser.actorId},
           |  "nickName": "${existingUser.nickName}",
           |  "displayName": "${existingUser.displayName}",
           |  "email": "${existingUser.email}"
           |}
          """.stripMargin
        ) else Unauthorized
      }
    }))
  }

  private def randomId() = Seq.fill(8)(Random.nextPrintableChar()).mkString
}
