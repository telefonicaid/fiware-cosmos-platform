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

package es.tid.cosmos.infinity.server.unfiltered.response

import javax.servlet.http.{HttpServletRequest, HttpServletResponse}
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import unfiltered.Async
import unfiltered.request.HttpRequest
import unfiltered.response.ResponseFunction

import es.tid.cosmos.infinity.server.content.UnfilteredExceptionRenderer

object Responder {

  type Request = HttpRequest[HttpServletRequest] with Async.Responder[HttpServletResponse]

  lazy val ExceptionRenderer = new UnfilteredExceptionRenderer[Request, HttpServletResponse]

  def respond(
      request: Request, response_> : Future[ResponseFunction[HttpServletResponse]]): Unit = {
    response_>.onSuccess { case response => request.respond(response) }
    response_>.onFailure { case e => request.respond(ExceptionRenderer(request, e)) }
  }

  def respond(request: Request, response: ResponseFunction[HttpServletResponse]): Unit = {
    request.respond(response)
  }

  def respond(request: Request, error: Throwable): Unit = {
    respond(request, ExceptionRenderer(request, error))
  }
}
