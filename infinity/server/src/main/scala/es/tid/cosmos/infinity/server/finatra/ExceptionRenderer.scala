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

import com.twitter.finatra.ResponseBuilder

import es.tid.cosmos.infinity.common.messages.ErrorDescriptor
import es.tid.cosmos.infinity.common.messages.json.ErrorDescriptorFormatter
import es.tid.cosmos.infinity.server.authentication.AuthenticationException

object ExceptionRenderer {

  private val errorFormatter = new ErrorDescriptorFormatter()

  def apply(exception: Throwable): ResponseBuilder = exception match {
    case e: RequestParsingException.MissingAuthorizationHeader =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.UnsupportedAuthorizationHeader =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.MalformedKeySecretPair =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.InvalidBasicHash =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.InvalidResourcePath =>
      render(401, ErrorCode(e), e)
    case e: AuthenticationException =>
      render(401, ErrorCode(e), e)
    case _ => throw new IllegalArgumentException(
      s"no rendering mechanism defined for ${exception.getClass.getCanonicalName}")
  }

  private def render[E <: Throwable](status: Int, errorCode: ErrorCode[E], exception: Throwable) =
    new ResponseBuilder()
      .status(status)
      .json(errorFormatter.format(ErrorDescriptor(errorCode.code, exception.getMessage)))

  private def renderWithAuth[E <: Throwable](
    status: Int, errorCode: ErrorCode[E], exception: Throwable) =
    render(status, errorCode, exception)
      .header("WWW-Authenticate", """Basic realm="Infinity", Bearer realm="Infinity"""")
}
