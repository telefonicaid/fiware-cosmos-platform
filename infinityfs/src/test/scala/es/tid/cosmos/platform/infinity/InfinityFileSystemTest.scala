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

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs._
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.util.Progressable
import org.mockito.BDDMockito.given
import org.mockito.Mockito.verify
import org.mockito.Matchers.{eq => the, _}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

class InfinityFileSystemTest extends FlatSpec with MustMatchers with MockitoSugar {

  val conf = new Configuration(true)
  conf.set("fs.infinity.defaultAuthority", "default:1234")
  val nestedFileSystem = mock[FileSystem]
  val permission = new FsPermission("755")
  val progressable = mock[Progressable]
  val replication: Short = 2
  val blockSize = 2048
  val bufferSize = 1024
  val inputStream = mock[FSDataInputStream]
  val outputStream = mock[FSDataOutputStream]

  /** Infinity FS initialized without by default to default:1234 */
  val defaultInfinity = new InfinityFileSystem(nestedFileSystem)

  /** Infinity FS initialized against a specific instance at host:8888 */
  val specificInfinity = new InfinityFileSystem(nestedFileSystem)

  "The Infinity filesystem" must "initialize the nested filesystem" in {
    defaultInfinity.initialize(URI.create("infinity:///path"), conf)
    verify(nestedFileSystem).initialize(URI.create("hdfs://default:1234/user/path"), conf)
    specificInfinity.initialize(URI.create("infinity://host:8888/path"), conf)
    verify(nestedFileSystem).initialize(URI.create("hdfs://host:8888/user/path"), conf)
  }

  it must "report infinity:// URIs" in {
    defaultInfinity.getUri must be (URI.create("infinity:///"))
  }

  it must "make paths qualified" in {
    given(nestedFileSystem.getWorkingDirectory).willReturn(new Path("hdfs://default:1234/user/foo"))
    defaultInfinity.makeQualified(new Path("infinity:///")) must be (new Path("infinity:///"))
  }

  it must "delegate `open` with translated path" in {
    given(nestedFileSystem.open(any[Path], the(bufferSize))).willReturn(inputStream)
    defaultInfinity.open(new Path("infinity:///path"), bufferSize) must be (inputStream)
    verify(nestedFileSystem).open(new Path("hdfs://default:1234/user/path"), bufferSize)
  }

  it must "delegate `create` with translated path" in {
    val overwrite = true
    given(nestedFileSystem.create(any[Path], any[FsPermission], anyBoolean, anyInt,
      anyShort, anyLong, any[Progressable])).willReturn(outputStream)
    defaultInfinity.create(new Path("infinity:///path"), permission, overwrite, bufferSize,
      replication, blockSize, progressable) must be (outputStream)
    verify(nestedFileSystem).create(new Path("hdfs://default:1234/user/path"), permission, overwrite,
      bufferSize, replication, blockSize, progressable)
  }

  it must "delegate `append` with translated path" in {
    given(nestedFileSystem.append(any[Path], anyInt, any[Progressable])).willReturn(outputStream)
    defaultInfinity.append(new Path("infinity://other/path"), bufferSize, progressable) must
      be (outputStream)
    verify(nestedFileSystem).append(new Path("hdfs://other/user/path"), bufferSize, progressable)
  }

  it must "delegate `rename` with translated path" in {
    given(nestedFileSystem.rename(any[Path], any[Path])).willReturn(true)
    defaultInfinity.rename(new Path("infinity:///from"), new Path("infinity:///to")) must be (true)
    verify(nestedFileSystem)
      .rename(new Path("hdfs://default:1234/user/from"), new Path("hdfs://default:1234/user/to"))
  }

  it must "delegate `delete` with translated path" in {
    given(nestedFileSystem.delete(any[Path], anyBoolean)).willReturn(true)
    defaultInfinity.delete(new Path("infinity://host/path"), recursive = true) must be (true)
    verify(nestedFileSystem).delete(new Path("hdfs://host/user/path"), true)
  }

  it must "delegate `listStatus` with translated listed path" in {
    val status = mock[FileStatus]
    given(status.getPath).willReturn(new Path("hdfs://host/user/path/file"))
    val statuses = Array(status)
    given(nestedFileSystem.listStatus(any[Path])).willReturn(statuses)
    val listing = defaultInfinity.listStatus(new Path("infinity:///path"))
    listing(0).getPath must be (new Path("infinity:///path/file"))
  }

  it must "delegate `setWorkingDirectory` with translated path" in {
    defaultInfinity.setWorkingDirectory(new Path("infinity:///path"))
    verify(nestedFileSystem).setWorkingDirectory(new Path("hdfs://default:1234/user/path"))
  }

  it must "delegate `getWorkingDirectory` translating back returned path" in {
    given(nestedFileSystem.getWorkingDirectory).willReturn(new Path("hdfs://host/user/path"))
    defaultInfinity.getWorkingDirectory must be (new Path("infinity:///path"))
  }

  it must "delegate `mkdirs` with translated path" in {
    given(nestedFileSystem.mkdirs(any[Path], any[FsPermission])).willReturn(true)
    defaultInfinity.mkdirs(new Path("infinity:///path"), permission) must be (true)
    verify(nestedFileSystem).mkdirs(new Path("hdfs://default:1234/user/path"), permission)
  }

  it must "delegate `getFileStatus` with translated status path" in {
    val status = mock[FileStatus]
    given(status.getPath).willReturn(new Path("hdfs://host/user/path"))
    given(nestedFileSystem.getFileStatus(any[Path])).willReturn(status)
    defaultInfinity.getFileStatus(new Path("infinity:///path")).getPath must
      be (new Path("infinity:///path"))
  }

  it must "return the correct scheme on getScheme" in {
    defaultInfinity.getScheme() must be ("infinity")
  }
}
