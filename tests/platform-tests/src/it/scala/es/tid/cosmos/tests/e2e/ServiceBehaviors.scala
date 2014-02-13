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

trait ServiceBehaviors { this: E2ETestBase =>

  type TestOnCluster = Cluster => Unit

  protected val service: String

  def installingServiceAndRunningAnExample(test: TestOnCluster) {
    withNewUser { user =>
      var cluster: Cluster = null

      scenario(s"The user can request a cluster with $service") {
        cluster = Cluster.create(2, user, services = Seq(service))
        cluster.isListed must be (true)
        cluster.state.get must (be ("provisioning") or be ("running"))
        cluster.ensureState("running")
      }

      scenario(s"The user can use $service on the cluster")(test(cluster))

      scenario("The user can terminate the cluster") {
        cluster.terminate()
        cluster.state.get must (be ("terminating") or be ("terminated"))
        cluster.ensureState("terminated")
      }
    }
  }
}
