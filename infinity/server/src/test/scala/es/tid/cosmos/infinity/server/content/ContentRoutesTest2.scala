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

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import unfiltered.filter.async.Plan.Intent

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.DataNode
import es.tid.cosmos.infinity.server.routes.RoutesBehavior
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

class ContentRoutesTest2 extends FlatSpec with RoutesBehavior with MustMatchers with FutureMatchers {

  def newRoutes: Routes = new Routes {
    val urlMapper = new InfinityUrlMapper(config)
    val config = new ContentServerConfig(ConfigFactory.load())
    val dataNode = mock[DataNode]("dataNode")
    val someUri = "/infinityfs/v1/content/some/uri"
    val somePath = Path.absolute("/some/uri")
    val authService = mock[AuthenticationService]
    val intent: Intent = new ContentRoutes(
      config,
      authService,
      dataNode,
      urlMapper
    ).intent
  }

  "GetContent" must behave like {
    supportsAuthorization(request = identity)
  }

  "AppendContent" must behave like {
    supportsAuthorization(request = _.copy(method = "POST"))
  }

  "OverwriteContent" must behave like {
    supportsAuthorization(request = _.copy(method = "PUT"))
  }
}
