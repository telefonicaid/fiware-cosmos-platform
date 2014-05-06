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

import java.io.FileNotFoundException

import org.apache.hadoop.security.AccessControlException

import es.tid.cosmos.infinity.server.authentication.AuthenticationException

class ErrorCode[T <: Throwable](val code: String)

object ErrorCode {

  implicit val MissingAuthorizationHeader =
    new ErrorCode[RequestParsingException.MissingAuthorizationHeader]("101")
  implicit val UnsupportedAuthorizationHeader =
    new ErrorCode[RequestParsingException.UnsupportedAuthorizationHeader]("102")
  implicit val MalformedKeySecretPair =
    new ErrorCode[RequestParsingException.MalformedKeySecretPair]("103")
  implicit val InvalidBasicHash =
    new ErrorCode[RequestParsingException.InvalidBasicHash]("104")
  implicit val InvalidResourcePath =
    new ErrorCode[RequestParsingException.InvalidResourcePath]("105")
  implicit val AuthenticationException =
    new ErrorCode[AuthenticationException]("106")
  implicit val AccessControlException =
    new ErrorCode[AccessControlException]("107")
  implicit val FileNotFoundException =
    new ErrorCode[FileNotFoundException]("108")

  def apply[E <: Throwable : ErrorCode](e: E): ErrorCode[E] = implicitly[ErrorCode[E]]
}
