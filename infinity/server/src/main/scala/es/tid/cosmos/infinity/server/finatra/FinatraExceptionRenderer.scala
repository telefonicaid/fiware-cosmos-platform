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

import es.tid.cosmos.infinity.server.errors.ExceptionRenderer

class FinatraExceptionRenderer extends ExceptionRenderer[ResponseBuilder] {
  override protected def withAuthHeader(response: ResponseBuilder): ResponseBuilder =
    response.header("WWW-Authenticate", """Basic realm="Infinity", Bearer realm="Infinity"""")

  override protected def render(status: Int, jsonContent: String): ResponseBuilder =
    new ResponseBuilder().status(status).json(jsonContent)
}
