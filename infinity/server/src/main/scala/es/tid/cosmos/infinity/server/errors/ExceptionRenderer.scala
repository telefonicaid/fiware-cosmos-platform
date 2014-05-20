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

package es.tid.cosmos.infinity.server.errors

import es.tid.cosmos.infinity.common.json.ErrorDescriptorFormatter
import es.tid.cosmos.infinity.common.messages.ErrorDescriptor
import es.tid.cosmos.infinity.server.authentication.AuthenticationException
import es.tid.cosmos.infinity.server.hadoop.{DataNodeException, NameNodeException}

trait ExceptionRenderer[Response] {
  import ExceptionRenderer._

  private val errorFormatter = new ErrorDescriptorFormatter()

  def apply(exception: Throwable): Response = exception match {
    case e: RequestParsingException.MissingAuthorizationHeader =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.UnsupportedAuthorizationHeader =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.MalformedKeySecretPair =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.InvalidBasicHash =>
      renderWithAuth(401, ErrorCode(e), e)
    case e: RequestParsingException.InvalidResourcePath =>
      render(404, ErrorCode(e), e)
    case e: RequestParsingException.InvalidRequestBody =>
      render(400, ErrorCode(e), e)
    case e: AuthenticationException =>
      render(401, ErrorCode(e), e)
    case e: NameNodeException.IOError =>
      render(500, ErrorCode(e), e)
    case e: NameNodeException.NoSuchPath =>
      render(404, ErrorCode(e), e)
    case e: DataNodeException.ContentNotFound =>
      render(404, ErrorCode(e), e)
    case e: NameNodeException.PathAlreadyExists =>
      render(409, ErrorCode(e), e)
    case e: NameNodeException.ParentNotDirectory =>
      render(422, ErrorCode(e), e)
    case e: NameNodeException.Unauthorized =>
      render(403, ErrorCode(e), e)
    case e =>
      render(500, ErrorCode.UnexpectedError, e)
  }

  private def render[E <: Throwable](
      status: Int, errorCode: ErrorCode[E], exception: Throwable): Response =
    render(status, errorFormatter.format(ErrorDescriptor(errorCode.code, exception.getMessage)))

  private def renderWithAuth[E <: Throwable](
      status: Int, errorCode: ErrorCode[E], exception: Throwable): Response =
    withAuthHeader(render(status, errorCode, exception), AuthHeaderContent)

  protected def withAuthHeader(response: Response, headerContent: String): Response

  protected def render(status: Int, jsonContent: String): Response
}

private object ExceptionRenderer {
  val AuthHeaderContent = """Basic realm="Infinity", Bearer realm="Infinity""""
}
