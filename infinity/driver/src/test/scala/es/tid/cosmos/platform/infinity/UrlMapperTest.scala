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

package es.tid.cosmos.platform.infinity

import java.net.URI

import org.apache.hadoop.fs.{FileStatus, Path}
import org.apache.hadoop.fs.permission.FsPermission
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class UrlMapperTest extends FlatSpec with MustMatchers {

  val mapperWithAuthority = new UrlMapper(Some("foo-infinity"))
  val mapperWithoutAuthority = new UrlMapper(None)

  "An Infinity URL" must "be transformed to a WebHDFS one" in {
    mapperWithAuthority.transform(URI.create("infinity://host/path")) must
      be (URI.create("webhdfs://host/user/path"))
  }

  it must "be transformed with default authority when appropriate" in {
    mapperWithAuthority.transform(URI.create("infinity:///path")) must
      be (URI.create("webhdfs://foo-infinity/user/path"))
    mapperWithoutAuthority.transform(URI.create("infinity:///path")) must
      be (URI.create("webhdfs:///user/path"))
  }

  "An Infinity path" must "be transformed to a WebHDFS one" in {
    mapperWithAuthority.transform(new Path("infinity://host/path")) must
      be (new Path("webhdfs://host/user/path"))
  }

  "A WebHDFS URL" must "be transformed back to an Infinity one" in {
    val expectedURI = URI.create("infinity:///path")
    mapperWithAuthority.transformBack(URI.create("webhdfs:///user/path")) must be (expectedURI)
    mapperWithoutAuthority.transformBack(URI.create("webhdfs:///user/path")) must be (expectedURI)
  }

  it must "fail if the URL does not contain the /user folder" in {
    evaluating {
      mapperWithAuthority.transformBack(URI.create("webhdfs:///path"))
    } must produce[IllegalArgumentException]
    evaluating {
      mapperWithoutAuthority.transformBack(URI.create("webhdfs:///path"))
    } must produce[IllegalArgumentException]
    evaluating {
      mapperWithoutAuthority.transformBack(URI.create("webhdfs:///path/user"))
    } must produce[IllegalArgumentException]
    evaluating {
      mapperWithoutAuthority.transformBack(URI.create("webhdfs:///path/user"))
    } must produce[IllegalArgumentException]
  }

  "A WebHDFS path" must "be transformed back to an Infinity one" in {
    mapperWithAuthority.transformBack(new Path("webhdfs:///user/path")) must
      be(new Path("infinity:///path"))
  }

  it must "fail if the URL does not contain the /user folder" in {
    evaluating {
      mapperWithAuthority.transformBack(new Path("webhdfs:///path"))
    } must produce[IllegalArgumentException]
    evaluating {
      mapperWithoutAuthority.transformBack(new Path("webhdfs:///path"))
    } must produce[IllegalArgumentException]
    evaluating {
      mapperWithoutAuthority.transformBack(new Path("webhdfs:///path/user"))
    } must produce[IllegalArgumentException]
    evaluating {
      mapperWithoutAuthority.transformBack(new Path("webhdfs:///path/user"))
    } must produce[IllegalArgumentException]
  }

  val len = 1000L
  val isDir = false
  val replication = 2
  val blockSize = 2048
  val time = 0L

  "A file status" must "have its path transformed back to an Infinity one" in {
    mapperWithAuthority.transformBack(new FileStatus(
      len, isDir, replication, blockSize, time, new Path("webhdfs://foo/user/path"))) must
      be (new FileStatus(len, isDir, replication, blockSize, time, new Path("infinity:///path")))
  }

  it must "transform back the symlink path for symlinks" in {
    val permission = new FsPermission("700")
    val originalFileStatus = new FileStatus(len, isDir, replication, blockSize, time, time, permission, "owner",
      "group", new Path("webhdfs://foo/user/link"), new Path("webhdfs://foo/user/path"))
    val transformedFileStatus = new FileStatus(len, isDir, replication, blockSize, time, time, permission, "owner",
      "group", new Path("infinity:///link"), new Path("infinity:///path"))
    mapperWithAuthority.transformBack(originalFileStatus) must be (transformedFileStatus)
  }
}
