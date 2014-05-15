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

package es.tid.cosmos.infinity.server.errors

import es.tid.cosmos.infinity.server.authentication.AuthenticationException
import es.tid.cosmos.infinity.server.hadoop.NameNodeException

class ErrorCode[T <: Throwable](val code: String)

object ErrorCode {

  /* Request errors. */
  implicit val MissingAuthorizationHeader =
    new ErrorCode[RequestParsingException.MissingAuthorizationHeader]("REQ01")
  implicit val InvalidResourcePath =
    new ErrorCode[RequestParsingException.InvalidResourcePath]("REQ02")
  implicit val UnsupportedAuthorizationHeader =
    new ErrorCode[RequestParsingException.UnsupportedAuthorizationHeader]("REQ03")
  implicit val MalformedKeySecretPair =
    new ErrorCode[RequestParsingException.MalformedKeySecretPair]("REQ04")
  implicit val InvalidBasicHash =
    new ErrorCode[RequestParsingException.InvalidBasicHash]("REQ05")
  implicit val InvalidRequestBody =
    new ErrorCode[RequestParsingException.InvalidRequestBody]("REQ06")

  /* Security errors. */
  implicit val AuthenticationFailed =
    new ErrorCode[AuthenticationException]("SEC01")
  implicit val Unauthorized =
    new ErrorCode[NameNodeException.Unauthorized]("SEC02")

  /* Server errors. */
  implicit val IOError =
    new ErrorCode[NameNodeException.IOError]("SRV01")

  /* Constraint errors. */
  implicit val NoSuchPath =
    new ErrorCode[NameNodeException.NoSuchPath]("CONST01")
  implicit val PathAlreadyExists =
    new ErrorCode[NameNodeException.PathAlreadyExists]("CONST02")
  implicit val ParentNotDirectory =
    new ErrorCode[NameNodeException.ParentNotDirectory]("CONST03")

  val UnexpectedError = new ErrorCode[Nothing]("BUG")

  def apply[E <: Throwable : ErrorCode](e: E): ErrorCode[E] = implicitly[ErrorCode[E]]
}
