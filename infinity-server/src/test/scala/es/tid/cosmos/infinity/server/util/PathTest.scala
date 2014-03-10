/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.infinity.server.util

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class PathTest extends FlatSpec with MustMatchers {

  "Filesystem path" must "be constructed from its parent and element name" in {
    new Path(Some(Path.Root), "foo") must be (Path / "foo")
  }

  it must "fail to construct from its parent and element name with separator" in {
    evaluating { new Path(Some(Path.Root), "foo/bar") } must produce [IllegalArgumentException]
  }

  it must "be constructed from absolute non-empty string value" in {
    Path.absolute("/foo/bar") must be (Path / "foo" / "bar")
  }

  it must "be constructed from absolute non-empty string value ignoring trailing separator" in {
    Path.absolute("/foo/bar/") must be (Path / "foo" / "bar")
  }

  it must "be constructed from absolute empty string value" in {
    Path.absolute("") must be (Path.Root)
  }

  it must "fail to construct from absolute string without leading separator" in {
    evaluating { Path.absolute("foo/bar/") } must produce [IllegalArgumentException]
  }

  it must "retrieve the right user directory" in {
    Path.userDirectory("apv") must be (Path / "user" / "apv")
  }

  it must "fail to retrieve the user directory for empty username" in {
    evaluating { Path.userDirectory("") } must produce [IllegalArgumentException]
  }

  it must "be constructed from non-empty relative string value" in {
    Path.relative(path = "foo/bar", username = "apv") must be (
      Path / "user" / "apv" / "foo" / "bar")
  }

  it must "be constructed from non-empty relative string value ignoring trailing separator" in {
    Path.relative(path = "foo/bar/", username = "apv") must be (
      Path / "user" / "apv" / "foo" / "bar")
  }

  it must "be constructed from empty relative string value" in {
    Path.relative(path = "", username = "apv") must be (
      Path / "user" / "apv")
  }

  it must "be converted into string" in {
    (Path / "foo" / "bar").toString must be ("/foo/bar")
  }

  it must "be converted into string when root" in {
    Path.Root.toString must be ("/")
  }
}
