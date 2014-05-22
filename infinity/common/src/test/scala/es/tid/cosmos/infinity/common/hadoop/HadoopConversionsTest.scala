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

package es.tid.cosmos.infinity.common.hadoop

import java.net.URL
import java.util.Date

import org.apache.hadoop.fs.{FileStatus, Path => HadoopPath}
import org.apache.hadoop.fs.permission.FsPermission
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.fs.{DirectoryMetadata, FileMetadata, Path, RootPath}
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HadoopConversionsTest extends FlatSpec with MustMatchers {

  import HadoopConversions._

  "An FsPermission" must "be converted to PermissionsMask" in {
    new FsPermission("755").toInfinity must be (PermissionsMask.fromOctal("755"))
  }

  "A PermissionsMask" must "be converted to FsPermission" in {
    PermissionsMask.fromOctal("755").toHadoop must be(new FsPermission("755"))
  }

  "Hadoop conversions" must "convert FS permission into Infinity with sticky bit" in {
    new FsPermission(PermissionsMask.fromOctal("1755").toShort).toInfinity must be (
      PermissionsMask.fromOctal("1755"))
  }

  it must "convert Infinity to FS permissions without sticky bit" in {
    PermissionsMask.fromOctal("755").toHadoop.toString must be("rwxr-xr-x")
  }

  it must "convert Infinity to FS permissions with sticky bit" in {
    PermissionsMask.fromOctal("1755").toHadoop.toString must be("rwxr-xr-t")
  }

  it must "be symmetric without sticky bit" in {
      PermissionsMask.fromOctal("755").toHadoop.toInfinity must be (PermissionsMask.fromOctal("755"))
  }

  it must "be symmetric with sticky bit" in {
    PermissionsMask.fromOctal("1755").toHadoop.toInfinity must be (PermissionsMask.fromOctal("1755"))
  }

  "A Hadoop path" must "be converted to an infinity one" in {
    new HadoopPath("/").toInfinity must be(RootPath)
    new HadoopPath("/foo/bar").toInfinity must be(RootPath / "foo" / "bar")
  }

  "An infinity path" must "be converted to a Hadoop one" in {
    RootPath.toHadoop must be (new HadoopPath("/"))
    Path.absolute("/foo/bar").toHadoop must be (new HadoopPath("/foo/bar"))
  }

  "A FileMetadata" must "be converted to a FileStatus" in {
    val mask = PermissionsMask.fromOctal("755")
    FileMetadata(
      path = Path.absolute("/some/file"),
      metadata = new URL("http://metadata/some/file"),
      content = new URL("http://content/some/file"),
      owner = "owner",
      group = "group",
      modificationTime = new Date(42),
      accessTime = new Date(43),
      permissions = mask,
      replication = 3,
      blockSize = 32,
      size = 1024
    ).toHadoop must be (new FileStatus(1024, false, 3, 32, 42, 43, mask.toHadoop, "owner", "group",
      new HadoopPath("/some/file")))
  }

  "A DirectoryMetadata" must "be converted to a FileStatus" in {
    val mask = PermissionsMask.fromOctal("1755")
    DirectoryMetadata(
      path = RootPath,
      metadata = new URL("http://metadata/"),
      content = Seq.empty,
      owner = "owner",
      group = "group",
      modificationTime = new Date(42),
      accessTime = new Date(43),
      permissions = mask
    ).toHadoop must be (new FileStatus(0, true, 0, 32, 42, 43, mask.toHadoop, "owner", "group",
      new HadoopPath("/")))
  }
}
