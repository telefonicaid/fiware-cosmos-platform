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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.UserState._

class UserStateTest extends FlatSpec with MustMatchers {

  "A user state" must "be convertible to string" in {
    UserState.values.map(_.toString).toList.sorted must equal (Seq(
      "creating", "deleted", "deleting", "disabled", "enabled"))
  }

  it must "be extracted from its string representation" in {
    UserState.unapply("disabled") must be (Some(Disabled))
  }

  it must "be extracted in case-insensitive fashion" in {
    UserState.unapply("DeleTed") must be (Some(Deleted))
  }

  it must "not be extracted from unknown strings" in {
    UserState.unapply("unknown") must not be 'defined
  }
}
