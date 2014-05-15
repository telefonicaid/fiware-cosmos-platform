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

import es.tid.cosmos.infinity.server.actions.MetadataAction._
import es.tid.cosmos.infinity.common.json.MetadataFormatter

/** An object able to render action results into HTTP responses. */
object ActionResultHttpRenderer {

  private val metadataFormatter = new MetadataFormatter

  def apply(result: Result): ResponseBuilder = result match {
    case Retrieved(metadata) => new ResponseBuilder()
      .status(200)
      .json(metadataFormatter.format(metadata))
    case Created(metadata) => new ResponseBuilder()
      .status(201)
      .json(metadataFormatter.format(metadata))
    case Moved(metadata) => new ResponseBuilder()
      .status(201)
      .json(metadataFormatter.format(metadata))
    case Deleted(_) | OwnerSet(_) | GroupSet(_)  | PermissionsSet(_) =>
      new ResponseBuilder().status(204)
    case _ => new ResponseBuilder()
      .status(500)
      .body("No HTTP rendering method defined for action result")
  }
}
