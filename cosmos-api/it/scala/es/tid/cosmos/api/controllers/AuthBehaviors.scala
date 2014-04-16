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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.http.Writeable
import play.api.mvc.Session
import play.api.test.FakeRequest
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.controllers.ResultMatchers._
import es.tid.cosmos.api.profile.ApiCredentials

trait AuthBehaviors { this: FlatSpec with MustMatchers =>

  def rejectingUnauthenticatedRequests[T](request: FakeRequest[T])(
      implicit evidence: Writeable[T]
    ) {
    it must "reject unauthenticated request" in new WithSampleSessions {
      status(route(request).get) must equal (UNAUTHORIZED)
    }
    it must "reject request with malformed credentials" in new WithSampleSessions {
      val invalidRequest = request.withHeaders("Authorization" -> "invalid")
      status(route(invalidRequest).get) must equal (UNAUTHORIZED)
    }

    it must "reject request with invalid credentials"in new WithSampleSessions {
      val userWithInvalidCredentials = new UserSession {
        val session = Session()
        val apiCredentials = Some(ApiCredentials.random())
      }
      status(userWithInvalidCredentials.doRequest(request)) must equal (UNAUTHORIZED)
    }
  }

  def pageForRegisteredUsers(path: String) {
    it must "redirect unauthenticated users to the index" in new WithSampleSessions {
      unauthUser.doRequest(path) must redirectTo ("/")
    }

    it must "redirect unregistered users to the registration page" in new WithSampleSessions {
      unregUser.doRequest(path) must redirectTo ("/register")
    }
  }

  def operatorOnlyResource[T: Writeable](request: FakeRequest[T]) {
    it must behave like rejectingUnauthenticatedRequests(request)

    it must "forbid requests from non-operator users" in new WithSampleSessions {
      status(regUserInGroup.doRequest(request)) must equal (FORBIDDEN)
    }

    it must "accept requests from operator users" in new WithSampleSessions {
      status(opUser.doRequest(request)) must not (equal (UNAUTHORIZED) or equal (FORBIDDEN))
    }
  }
}
