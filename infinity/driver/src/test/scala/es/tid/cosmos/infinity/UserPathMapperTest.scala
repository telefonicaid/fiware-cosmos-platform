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

package es.tid.cosmos.infinity

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.fs.RootPath

class UserPathMapperTest extends FlatSpec with MustMatchers {

  "User paths" must "map to a subpath of /user" in {
    UserPathMapper.absolutePath(RootPath) must be (RootPath / "user")
    UserPathMapper.absolutePath(RootPath / "foo") must be (RootPath / "user" / "foo")
  }

  it must "be mapped back from absolute paths" in {
    UserPathMapper.userPath(RootPath / "user") must be (RootPath)
    UserPathMapper.userPath(RootPath / "user" / "foo") must be (RootPath / "foo")
  }

  it must "throw when transforming back is not possible" in {
    evaluating {
      UserPathMapper.userPath(RootPath / "other_dir")
    } must produce [IllegalArgumentException]
  }
}
