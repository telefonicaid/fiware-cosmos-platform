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
package es.tid.cosmos.infinity.server.actions

import com.twitter.finatra.FinatraServer
import com.twitter.finatra.test.SpecHelper
import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers

class MetadataActionsTest extends FlatSpec with ShouldMatchers with SpecHelper {

    val app = new MetadataActions
    override val server = new FinatraServer
    server.register(app)

    "Get file metadata" should "respond 200" in {
      get("/infinityfs/v1/metadata/some/file.txt")
      response.body should equal ("metadata of /some/file.txt")
      response.code should equal (200)
    }

  }
