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

package es.tid.cosmos.servicemanager.ambari.services

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ComponentDescriptionTest extends FlatSpec with MustMatchers {

  "A component description" must "require at least one component location" in {
    evaluating {
      ComponentDescription("foo", Set.empty)
    } must produce [IllegalArgumentException]
  }

  it must "be turned client side" in {
    val nonClientComponent = ComponentDescription.masterComponent("foo")
    nonClientComponent must not be 'client
    nonClientComponent.makeClient must be('client)
  }

  it must "be considered master if any of its locations is master" in {
    ComponentDescription.masterComponent("foo") must be ('master)
    ComponentDescription.allNodesComponent("foo") must be ('master)
  }

  it must "be considered slave if any of its locations is slave" in {
    ComponentDescription.slaveComponent("foo") must be ('slave)
    ComponentDescription.allNodesComponent("foo") must be ('slave)
  }
}
