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

sealed abstract class RequestParsingException(msg: String, cause: Throwable = null) extends Exception(msg)

object RequestParsingException {

  case class MissingAuthorizationHeader(cause: Throwable = null) extends RequestParsingException(
    "no Authorization header was found in request", cause)

  case class UnsupportedAuthorizationHeader(headerValue: String, cause: Throwable = null)
    extends RequestParsingException(s"unsupported authorization header in '$headerValue'", cause)

  case class MalformedKeySecretPair(pair: String, cause: Throwable = null) extends RequestParsingException(
    s"invalid API key-secret pair in $pair", cause)

  case class InvalidBasicHash(hash: String, cause: Throwable = null) extends RequestParsingException(
    s"basic hash $hash is not a valid base64 encoded string", cause)

  case class InvalidResourcePath(path: String, cause: Throwable = null) extends RequestParsingException(
    s"invalid resource path $path", cause)

  case class InvalidRequestBody(body: String, cause: Throwable = null) extends RequestParsingException(
    s"invalid request body: \n$body", cause)
}
