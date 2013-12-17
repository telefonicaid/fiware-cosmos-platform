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
import es.tid.cosmos.servicemanager.clusters.ClusterId

/**
 * @author sortega
 */
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
    val instance0 = ClusterId()
    val instance1 = ClusterId(instance0.id)
    instance0 must equal (instance1)
    instance0.hashCode() must equal (instance1.hashCode())
  }
}
