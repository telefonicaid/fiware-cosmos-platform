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

package es.tid.cosmos.infinity.common.messages

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class PathTypeTest extends FlatSpec with MustMatchers {

  "Path type" must "be parsed from string" in {
    PathType.valueOf("file") must be (PathType.File)
    PathType.valueOf("directory") must be (PathType.Directory)
  }

  it must "be not parsed from unknown strings" in {
    evaluating(PathType.valueOf("unknown")) must produce [IllegalArgumentException]
  }
}
