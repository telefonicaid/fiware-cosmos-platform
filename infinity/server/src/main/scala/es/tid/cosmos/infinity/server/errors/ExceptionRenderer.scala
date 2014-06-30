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

import org.apache.commons.logging.LogFactory

import es.tid.cosmos.infinity.common.json.ErrorDescriptorFormatter
import es.tid.cosmos.infinity.common.messages.ErrorDescriptor
import es.tid.cosmos.infinity.server.authentication.AuthenticationException
import es.tid.cosmos.infinity.server.hadoop.{HdfsException, DataNodeException, NameNodeException}

trait ExceptionRenderer[Request, Response] {
  import ExceptionRenderer._

  private val errorFormatter = new ErrorDescriptorFormatter()

  def apply(request: Request, exception: Throwable): Response = exception match {
    case e: RequestParsingException.MissingAuthorizationHeader =>
      renderWithAuth(request, 401, ErrorCode(e), e)
    case e: RequestParsingException.UnsupportedAuthorizationHeader =>
      renderWithAuth(request, 401, ErrorCode(e), e)
    case e: RequestParsingException.MalformedKeySecretPair =>
      renderWithAuth(request, 401, ErrorCode(e), e)
    case e: RequestParsingException.InvalidBasicHash =>
      renderWithAuth(request, 401, ErrorCode(e), e)
    case e: RequestParsingException.InvalidResourcePath =>
      render(request, 404, ErrorCode(e), e)
    case e: RequestParsingException.InvalidRequestBody =>
      render(request, 400, ErrorCode(e), e)
    case e: AuthenticationException =>
      render(request, 401, ErrorCode(e), e)
    case e: HdfsException.IOError =>
      render(request, 500, ErrorCode(e), e)
    case e: NameNodeException.NoSuchPath =>
      render(request, 404, ErrorCode(e), e)
    case e: DataNodeException.FileNotFound =>
      render(request, 404, ErrorCode(e), e)
    case e: NameNodeException.PathAlreadyExists =>
      render(request, 409, ErrorCode(e), e)
    case e: NameNodeException.ParentNotDirectory =>
      render(request, 422, ErrorCode(e), e)
    case e: HdfsException.Unauthorized =>
      render(request, 403, ErrorCode(e), e)
    case e: DataNodeException.ContentPathIsDirectory =>
      render(request, 400, ErrorCode(e), e)
    case e =>
      Log.error("Unexpected error", e)
      render(request, 500, ErrorCode.UnexpectedError, e)
  }

  private def render[E <: Throwable](
      request: Request, status: Int, errorCode: ErrorCode[E], exception: Throwable): Response =
    render(request, status,
      errorFormatter.format(ErrorDescriptor(errorCode.code, exception.getMessage)))

  private def renderWithAuth[E <: Throwable](
      request: Request, status: Int, errorCode: ErrorCode[E], exception: Throwable): Response =
    withAuthHeader(render(request, status, errorCode, exception), AuthHeaderContent)

  protected def withAuthHeader(response: Response, headerContent: String): Response

  protected def render(request: Request, status: Int, jsonContent: String): Response
}

private object ExceptionRenderer {
  val Log = LogFactory.getLog(classOf[ExceptionRenderer[_, _]])
  val AuthHeaderContent = """Basic realm="Infinity", Bearer realm="Infinity""""
}
