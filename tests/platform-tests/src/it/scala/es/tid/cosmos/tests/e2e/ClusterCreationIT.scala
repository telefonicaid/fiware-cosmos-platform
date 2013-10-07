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

package es.tid.cosmos.tests.e2e

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.tags.EndToEndTest

class ClusterCreationIT extends FlatSpec with MustMatchers {
  var cluster: Cluster = null

  "A big cluster with default services" must "be created" taggedAs EndToEndTest in {
    cluster = Cluster.create(6)
    cluster.isListed must be (true)
    cluster.state.get must (be ("provisioning") or be ("running"))
    cluster.ensureState("running")
  }

  it must "be terminated" taggedAs EndToEndTest in {
    cluster.terminate()
    cluster.state.get must (be ("terminating") or be ("terminated"))
    cluster.ensureState("terminated")
  }

}
