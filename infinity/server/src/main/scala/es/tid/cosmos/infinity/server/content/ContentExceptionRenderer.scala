/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.content

import unfiltered.response._

import es.tid.cosmos.infinity.common.json.ErrorDescriptorFormatter
import es.tid.cosmos.infinity.common.messages.ErrorDescriptor
import es.tid.cosmos.infinity.server.actions.NameNodeException
import es.tid.cosmos.infinity.server.authentication.AuthenticationException
import es.tid.cosmos.infinity.server.errors.{ErrorCode, RequestParsingException}

object ContentExceptionRenderer {
  private val errorFormatter = new ErrorDescriptorFormatter()

  //TODO: Merge with Metadata ExceptionRenderer

  def apply[T](exception: Throwable): ResponseFunction[T] = exception match {
    case e: RequestParsingException.MissingAuthorizationHeader =>
      renderWithAuth(Unauthorized, ErrorCode(e), e)
    case e: RequestParsingException.UnsupportedAuthorizationHeader =>
      renderWithAuth(Unauthorized, ErrorCode(e), e)
    case e: RequestParsingException.MalformedKeySecretPair =>
      renderWithAuth(Unauthorized, ErrorCode(e), e)
    case e: RequestParsingException.InvalidBasicHash =>
      renderWithAuth(Unauthorized, ErrorCode(e), e)
    case e: RequestParsingException.InvalidResourcePath =>
      render(NotFound, ErrorCode(e), e)
    case e: AuthenticationException =>
      render(Unauthorized, ErrorCode(e), e)
    case e: NameNodeException.IOError =>
      render(InternalServerError, ErrorCode(e), e)
    case e: NameNodeException.NoSuchPath =>
      render(NotFound, ErrorCode(e), e)
    case e: NameNodeException.PathAlreadyExists =>
      render(Conflict, ErrorCode(e), e)
    case e: NameNodeException.ParentNotDirectory =>
      render(UnprocessableEntity, ErrorCode(e), e)
    case e: NameNodeException.Unauthorized =>
      render(Forbidden, ErrorCode(e), e)
    case _ => throw new IllegalArgumentException(
      s"no rendering mechanism defined for ${exception.getClass.getCanonicalName}")
  }

  private def render[E <: Throwable](status: Status, errorCode: ErrorCode[E], exception: Throwable) =
    status ~> ResponseString(
      errorFormatter.format(ErrorDescriptor(errorCode.code, exception.getMessage)))

  private def renderWithAuth[E <: Throwable](status: Status, errorCode: ErrorCode[E], exception: Throwable) =
    render(status, errorCode, exception) ~>
      WWWAuthenticate("""Basic realm="Infinity", Bearer realm="Infinity"""")

}
