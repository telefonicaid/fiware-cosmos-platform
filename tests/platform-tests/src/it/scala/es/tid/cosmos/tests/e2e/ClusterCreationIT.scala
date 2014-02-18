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

class ClusterCreationIT extends E2ETestBase {
  withNewUser { user =>
    feature("A user can manage clusters") {
      var cluster: Cluster = null

      scenario("The user can create a cluster") {
        cluster = new Cluster(6, user)
        cluster.isListed() must be (true)
        cluster.state().get must (be ("provisioning") or be ("running"))
        cluster.ensureState("running")
      }

      scenario("The user can terminate the cluster") {
        cluster.terminate()
        cluster.state().get must (be ("terminating") or be ("terminated"))
        cluster.ensureState("terminated")
      }
    }
  }
}
