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

package es.tid.cosmos.common.scalatest.resources

/** Mixin utility to gain access to test-resource directory paths. */
trait TestResourcePaths {
  /** Exposes the src/[test, it]/resources directory path */
  protected lazy val resourcesConfigDirectory = this.getClass.getClassLoader.getResource("").getPath

  /** Exposes the src/[test, it]/resources/a/package/path directory as the resource path based on the
    * test's package.
    */
  protected lazy val packageResourcesConfigDirectory = this.getClass.getResource("").getPath
}
