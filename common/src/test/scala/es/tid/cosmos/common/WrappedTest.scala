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

package es.tid.cosmos.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class WrappedTest extends FlatSpec with MustMatchers {
  "Wrapped" must "extract the cause of an exception" in {
    val innerEx = new NoSuchElementException("missing foo")
    val outerEx = new RuntimeException("something went wrong", innerEx)
    Wrapped.unapply(outerEx) must be (Some(innerEx))
  }

  it must "extract nothing from exceptions without cause" in {
    val simpleEx = new RuntimeException("simply failed")
    Wrapped.unapply(simpleEx) must not be 'defined
  }

  it must "extract nothing from null" in {
    Wrapped.unapply(null) must not be 'defined
  }
}
