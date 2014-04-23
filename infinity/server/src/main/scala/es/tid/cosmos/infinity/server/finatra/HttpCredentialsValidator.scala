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

package es.tid.cosmos.infinity.server.finatra

import java.net.InetAddress
import scalaz.Validation

import com.twitter.finagle.http.Request
import org.apache.commons.codec.binary.{Base64, StringUtils}

import es.tid.cosmos.infinity.common.credentials.{UserCredentials, Credentials, ClusterCredentials}

object HttpCredentialsValidator {

  import scalaz.Scalaz._

  private val base64 = new Base64()
  private val basicLinePattern = "Basic (.*)".r
  private val basicPairPattern = "(.*):(.*)".r
  private val bearerLinePattern = "Bearer (.*)".r

  def apply(
      from: InetAddress, request: Request): Validation[RequestError, Credentials] = {
    request.headers().get("Authorization") match {
      case basicLinePattern(hash) => userCredentials(hash)
      case bearerLinePattern(secret) => clusterCredentials(from, secret)
      case null => MissingAuthorizationHeader().failure
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
