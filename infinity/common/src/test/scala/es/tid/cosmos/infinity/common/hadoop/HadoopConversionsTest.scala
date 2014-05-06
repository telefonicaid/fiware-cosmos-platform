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

import org.apache.hadoop.fs.{Path => HadoopPath}
import org.apache.hadoop.fs.permission.FsPermission
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.RootPath
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HadoopConversionsTest extends FlatSpec with MustMatchers {

  import HadoopConversions._

  "HDFS conversions" must "convert FS permissions from HDFS into Infinity" in {
    new FsPermission("755").toInfinity must be (PermissionsMask.fromOctal("755"))
  }

  it must "convert FS permissions from Infinity into HDFS" in {
    PermissionsMask.fromOctal("755").toHadoop must be(new FsPermission("755"))
  }

  "A hadoop path" must "be converted to an infinity one" in {
    new HadoopPath("/").toInfinity must be (RootPath)
    new HadoopPath("/foo/bar").toInfinity must be (RootPath / "foo" / "bar")
  }
}
