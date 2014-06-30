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

package es.tid.cosmos.servicemanager

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.clusters.ClusterId

class ClusterIdTest extends FlatSpec with MustMatchers {

  "ClusterId instances" must "be equal by value" in {
    val instance0 = ClusterId("00000000000000000000000000000000")
    val instance1 = ClusterId("00000000000000000000000000000000")
    instance0 must equal (instance1)
    instance0.hashCode() must equal (instance1.hashCode())
  }

  it must "only contain alphanumeric characters" in {
    evaluating {
      ClusterId("abc-123")
    } must produce [IllegalArgumentException]
  }

  "ClusterId of random UUID" must "be equal by value with ClusterId with same UUID" in {
    val instance0 = ClusterId.random()
    val instance1 = ClusterId(instance0.id)
    instance0 must equal (instance1)
    instance0.hashCode() must equal (instance1.hashCode())
  }
}
