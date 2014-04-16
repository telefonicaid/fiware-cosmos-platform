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

package es.tid.cosmos.infinity.server.finatra

import java.net.InetAddress
import scalaz.Validation

import com.twitter.finagle.http.Request
import org.apache.commons.codec.binary.{Base64, StringUtils}

import es.tid.cosmos.infinity.server.authentication._

object HttpCredentialsValidator {

  import scalaz.Scalaz._

  abstract class InvalidHttpCredentials(msg: String) extends Exception(msg)

  case object MissingAuthorizationHeader extends InvalidHttpCredentials(
    "no Authorization header was found in request")

  case class UnsupportedAuthorizationHeader(headerValue: String) extends InvalidHttpCredentials(
    s"unsupported authorization header in '$headerValue'")

  case class MalformedKeySecretPair(pair: String) extends InvalidHttpCredentials(
    s"invalid API key-secret pair in $pair")

  case class InvalidBasicHash(hash: String) extends InvalidHttpCredentials(
    s"basic hash $hash is not a valid base64 encoded string")

  private val base64 = new Base64()
  private val basicLinePattern = "Basic (.*)".r
  private val basicPairPattern = "(.*):(.*)".r
  private val bearerLinePattern = "Bearer (.*)".r

  def apply(
      from: InetAddress, request: Request): Validation[InvalidHttpCredentials, Credentials] = {
    request.headers().get("Authorization") match {
      case basicLinePattern(hash) => userCredentials(hash)
      case bearerLinePattern(secret) => clusterCredentials(from, secret)
      case null => MissingAuthorizationHeader.failure
      case headerValue => UnsupportedAuthorizationHeader(headerValue).failure
    }
  }
  
  private def userCredentials(hash: String): Validation[InvalidHttpCredentials, UserCredentials] = {
    for {
      pair <- decodeBase64(hash)
      splitted <- splitPair(pair)
      (key, secret) = splitted
    } yield UserCredentials(key, secret)
  }

  private def decodeBase64(hash: String): Validation[InvalidHttpCredentials, String] = {
    val decoded = StringUtils.newStringUtf8(base64.decode(hash))
    if (decoded.isEmpty) InvalidBasicHash(hash).failure
    else decoded.success
  }

  private def splitPair(pair: String): Validation[InvalidHttpCredentials, (String, String)] =
    pair match {
      case basicPairPattern(key, secret) => (key, secret).success
      case _ => MalformedKeySecretPair(pair).failure
    }

  private def clusterCredentials(
      from: InetAddress,
      secret: String): Validation[InvalidHttpCredentials, ClusterCredentials] = {
    ClusterCredentials(from, secret).success
  }
}
