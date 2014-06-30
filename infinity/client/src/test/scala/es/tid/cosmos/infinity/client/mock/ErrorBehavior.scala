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

package es.tid.cosmos.infinity.client.mock

import javax.servlet.http.HttpServletResponse

import unfiltered.filter.{Plan, Planify}
import unfiltered.response._

import es.tid.cosmos.infinity.common.fs.{Path, PathMetadata}

object ErrorBehavior extends Formatters {

  abstract class SimpleBehavior(response: ResponseFunction[HttpServletResponse]) extends Behavior {
    override def planFor(
        paths: Map[Path, PathMetadata], contents: Map[Path, String], dataFactory: DataFactory): Plan =
      Planify { case _ => response }
  }

  object BadRequestBehavior extends SimpleBehavior(BadRequest ~> errorBody("don't understand you"))
  object GibberishResponseBehavior extends SimpleBehavior(Ok ~> ResponseString("dlalkdkldijodjd"))
  object ForbiddenBehavior extends SimpleBehavior(Forbidden ~> errorBody("don't dare trying that"))
  object AlreadyExistsBehavior
    extends SimpleBehavior(Conflict ~> errorBody("file or directory already exists"))
  object NotFoundBehavior
    extends SimpleBehavior(NotFound ~> errorBody("file or directory does not exist"))
}
