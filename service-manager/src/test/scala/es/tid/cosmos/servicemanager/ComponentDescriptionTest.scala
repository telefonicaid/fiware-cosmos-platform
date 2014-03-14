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

package es.tid.cosmos.servicemanager

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.servicemanager.ambari.services.ComponentDescription

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
