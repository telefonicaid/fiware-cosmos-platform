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

package es.tid.cosmos.infinity.server.metadata

import java.io.StringReader

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import unfiltered.filter.async.Plan.Intent

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.hadoop.NameNode
import es.tid.cosmos.infinity.server.routes.RoutesBehavior
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

class MetadataRoutesTest2 extends FlatSpec with RoutesBehavior with MustMatchers with FutureMatchers {

  "GetMetadata" must behave like {
    supportsAuthorization(request = identity)
  }

  "CreateFile" must behave like {
    supportsAuthorization(request = _.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "mkfile",
          |  "name": "somefile.txt",
          |  "permissions": "777",
          |  "replication": 3
          |}
        """.stripMargin)
    ))
  }

  "MoveFile" must behave like {
    supportsAuthorization(request = _.copy(
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "move",
          |  "name": "somefile.txt",
          |  "from": "/some/other/place"
          |}
        """.stripMargin)
    ))
  }

  def newRoutes = new Routes {
    val urlMapper = new InfinityUrlMapper(config)
    val config = new MetadataServerConfig(ConfigFactory.load())
    val nameNode = mock[NameNode]("nameNode")
    val someUri = "/infinityfs/v1/metadata/some/uri"
    val somePath = Path.absolute("/some/uri")
    val authService = mock[AuthenticationService]
    val intent: Intent = new MetadataRoutes(
      config,
      authService,
      nameNode,
      urlMapper
    ).intent
  }
}
