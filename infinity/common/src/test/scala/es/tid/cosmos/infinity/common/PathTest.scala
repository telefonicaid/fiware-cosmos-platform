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

package es.tid.cosmos.infinity.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class PathTest extends FlatSpec with MustMatchers {

  "Filesystem path" must "be constructed from its parent and element name" in {
    Path(RootPath, "foo") must be (RootPath / "foo")
  }

  it must "fail to construct from its parent and element name with separator" in {
    evaluating { Path(RootPath, "foo/bar") } must produce [IllegalArgumentException]
  }

  it must "be constructed from absolute non-empty string value" in {
    Path.absolute("/foo/bar") must be (RootPath / "foo" / "bar")
  }

  it must "be constructed from absolute non-empty string value ignoring trailing separator" in {
    Path.absolute("/foo/bar/") must be (RootPath / "foo" / "bar")
  }

  it must "be constructed from absolute empty string value" in {
    Path.absolute("") must be (RootPath)
  }

  it must "fail to construct from absolute string without leading separator" in {
    evaluating { Path.absolute("foo/bar/") } must produce [IllegalArgumentException]
  }

  it must "retrieve the right user directory" in {
    Path.userDirectory("apv") must be (RootPath / "user" / "apv")
  }

  it must "fail to retrieve the user directory for empty username" in {
    evaluating { Path.userDirectory("") } must produce [IllegalArgumentException]
  }

  it must "be constructed from non-empty relative string value" in {
    Path.relative(path = "foo/bar", username = "apv") must be (
      RootPath / "user" / "apv" / "foo" / "bar")
  }

  it must "be constructed from non-empty relative string value ignoring trailing separator" in {
    Path.relative(path = "foo/bar/", username = "apv") must be (
      RootPath / "user" / "apv" / "foo" / "bar")
  }

  it must "be constructed from empty relative string value" in {
    Path.relative(path = "", username = "apv") must be (
      RootPath / "user" / "apv")
  }

  it must "be converted into string" in {
    (RootPath / "foo" / "bar").toString must be ("/foo/bar")
  }

  it must "be converted into string when root" in {
    RootPath.toString must be ("/")
  }
}
