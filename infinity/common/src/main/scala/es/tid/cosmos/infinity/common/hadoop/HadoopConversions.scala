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

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

/** This object provides implicits to bling Hadoop objects into their equivalent Infinity ones. */
object HadoopConversions {

  implicit class FsPermissionConversion(permission: FsPermission) {
    def toInfinity: PermissionsMask = PermissionsMask.fromShort(permission.toShort)
  }

  implicit class PermissionsMaskConversion(permission: PermissionsMask) {
    def toHadoop: FsPermission = new FsPermission(permission.toShort)
  }

  implicit class HadoopPathConversion(val path: HadoopPath) extends AnyVal {
    def toInfinity: Path = Path.absolute(path.toUri.getPath)
  }
}
