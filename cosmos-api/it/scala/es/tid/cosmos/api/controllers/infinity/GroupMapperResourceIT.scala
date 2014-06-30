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

package es.tid.cosmos.api.controllers.infinity

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.common.BearerToken

class GroupMapperResourceIT extends FlatSpec with MustMatchers {

  "Group mapping" must "be rejected when not authenticated" in new Fixture {
    status(doRequest(handle = Some(opUser.handle), authHeader = None)) must be (UNAUTHORIZED)
  }

  it must "require a handle" in new Fixture {
    status(doRequest(handle = None)) must be (BAD_REQUEST)
  }

  it must "require a handle of an existing user profile" in new Fixture {
    status(doRequest(handle = Some("unexisting"))) must be (BAD_REQUEST)
  }

  it must "return the groups of a user profile" in new Fixture {
    val res = doRequest(handle = Some(regUserInGroup.handle))
    status(res) must be (OK)
    contentAsJson(res) must be (Json.toJson(UserGroups(Seq(regUserInGroup.group.name))))
  }

  it must "return the group cosmos for users in the NoGroup" in new Fixture {
    val res = doRequest(handle = Some(regUserNoGroup.handle))
    status(res) must be (OK)
    contentAsJson(res) must be (Json.toJson(UserGroups(Seq("cosmos"))))
  }

  trait Fixture extends WithSampleSessions {
    val validAuth = "Authorization" -> BearerToken("infinitypass")

    def doRequest(
        handle: Option[String],
        authHeader: Option[(String, String)] = Some(validAuth)) = {
      val queryString = handle.map(h => s"?handle=$h").getOrElse("")
      val request = FakeRequest(GET, "/infinity/v1/groups" + queryString)
        .withHeaders(authHeader.toSeq: _*)
      route(request).get
    }
  }
}
