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

package es.tid.cosmos.common.scalatest

import org.scalatest.Tag

/** Package containing test tag definitions used for test filtering. */
package object tags {

  /** Tag for tests with external dependencies requiring a special running environment
    * e.g. Database instances, system libraries etc.
    */
  object HasExternalDependencies extends Tag("HasExternalDependencies")

  /** Tag for end to end tests which are meant to be run against a functional instance of Cosmos */
  object EndToEndTest extends Tag("EndToEndTest")
}
