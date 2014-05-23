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

import es.tid.cosmos.infinity.common.fs.{Path, RootPath, SubPath}

/** It translates from paths as seen by the infinity users to the real ones (transform)
  * and back (transformBack).
  */
private[infinity] object UserPathMapper {

  val BasePath = RootPath / "user"

  def absolutePath(userPath: Path): SubPath = (BasePath / userPath).asInstanceOf[SubPath]

  def userPath(absolutePath: Path): Path = absolutePath match {
    case BasePath => RootPath
    case SubPath(parent, name) => userPath(parent) / name
    case other => throw new IllegalArgumentException(s"$other is not a subpath of $BasePath")
  }
}
