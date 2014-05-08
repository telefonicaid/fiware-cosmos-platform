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

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.actions.Action.ContentFound

/** An object able to render action results into HTTP responses. */
object ActionResultHttpRenderer {

  def apply(result: Action.Result): ResponseBuilder = result match {
    case ContentFound(stream, upTo) => new StreamResponseBuilder().body(stream, upTo).status(200)
    case _ => new ResponseBuilder()
      .status(500)
      .body("The ActionResultHttpRenderer class needs to be implemented")
  }
}
