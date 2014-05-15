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

package es.tid.cosmos.infinity.server.authorization

import java.net.InetAddress
import scala.util.Try

import org.apache.commons.codec.binary.{Base64, StringUtils}

import es.tid.cosmos.infinity.common.credentials.{UserCredentials, Credentials, ClusterCredentials}
import es.tid.cosmos.infinity.server.errors.RequestParsingException

object HttpCredentialsValidator {
  private val base64 = new Base64()
  private val basicLinePattern = "Basic (.*)".r
  private val basicPairPattern = "(.*):(.*)".r
  private val bearerLinePattern = "Bearer (.*)".r

  def apply(info: AuthInfo): Try[Credentials] = Try {
    info.header match {
      case basicLinePattern(hash) =>
        userCredentials(hash)
      case bearerLinePattern(secret) =>
        clusterCredentials(info.from, secret)
      case null =>
        throw RequestParsingException.MissingAuthorizationHeader()
      case headerValue =>
        throw RequestParsingException.UnsupportedAuthorizationHeader(headerValue)
    }
  }

  private def userCredentials(hash: String) = {
    val pair = decodeBase64(hash)
    val (key, secret) = splitPair(pair)
    UserCredentials(key, secret)
  }

  private def decodeBase64(hash: String) = {
    val decoded = StringUtils.newStringUtf8(base64.decode(hash))
    if (decoded.isEmpty) throw RequestParsingException.InvalidBasicHash(hash)
    else decoded
  }

  private def splitPair(pair: String) =
    pair match {
      case basicPairPattern(key, secret) => (key, secret)
      case _ => throw RequestParsingException.MalformedKeySecretPair(pair)
    }

  private def clusterCredentials(from: InetAddress, secret: String): ClusterCredentials =
    ClusterCredentials(from, secret)
}
