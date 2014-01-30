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

package es.tid.cosmos.platform.infinity

import java.net.URI

import org.apache.hadoop.fs.{FileStatus, Path}
import org.apache.hadoop.fs.permission.FsPermission
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class UrlMapperTest extends FlatSpec with MustMatchers {

  val mapperWithAuthority = new UrlMapper(Some("foo-infinity"))
  val mapperWithoutAuthority = new UrlMapper(None)

  "An Infinity URL" must "be transformed to a HDFS one" in {
    mapperWithAuthority.transform(URI.create("infinity://host/path")) must
      be (URI.create("hdfs://host/path"))
  }

  it must "be transformed with default authority when appropriate" in {
    mapperWithAuthority.transform(URI.create("infinity:///path")) must
      be (URI.create("hdfs://foo-infinity/path"))
    mapperWithoutAuthority.transform(URI.create("infinity:///path")) must
      be (URI.create("hdfs:///path"))
  }

  "An Infinity path" must "be transformed to a HDFS one" in {
    mapperWithAuthority.transform(new Path("infinity://host/path")) must
      be (new Path("hdfs://host/path"))
  }

  "A HDFS URL" must "be transformed back to an Infinity one" in {
    val expectedURI = URI.create("infinity:///path")
    mapperWithAuthority.transformBack(URI.create("hdfs:///path")) must be (expectedURI)
    mapperWithoutAuthority.transformBack(URI.create("hdfs:///path")) must be (expectedURI)
  }

  "A HDFS path" must "be transformed back to an Infinity one" in {
    mapperWithAuthority.transformBack(new Path("hdfs:///path")) must
      be(new Path("infinity:///path"))
  }

  val len = 1000L
  val isDir = false
  val replication = 2
  val blockSize = 2048
  val time = 0L

  "A file status" must "have its path transformed back to an Infinity one" in {
    mapperWithAuthority.transformBack(new FileStatus(
      len, isDir, replication, blockSize, time, new Path("hdfs://foo/path"))) must
      be (new FileStatus(len, isDir, replication, blockSize, time, new Path("infinity://foo/path")))
  }

  it must "transform back the symlink path for symlinks" in {
    val permission = new FsPermission("700")
    val originalFileStatus = new FileStatus(len, isDir, replication, blockSize, time, time, permission, "owner",
      "group", new Path("hdfs://link"), new Path("hdfs://foo/path"))
    val transformedFileStatus = new FileStatus(len, isDir, replication, blockSize, time, time, permission, "owner",
      "group", new Path("infinity://link"), new Path("infinity://foo/path"))
    mapperWithAuthority.transformBack(originalFileStatus) must be (transformedFileStatus)
  }
}
