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

class HiveServiceIT extends E2ETestBase {
  withNewUser { user =>
    feature("Hive is a supported service") {
      var cluster: Cluster = null

      scenario("The user can request a cluster with Hive") {
        cluster = Cluster.create(2, user, services = Seq("HIVE"))
        cluster.isListed must be (true)
        cluster.state.get must (be ("provisioning") or be ("running"))
        cluster.ensureState("running")
      }

      scenario("The user can use Hive on the cluster") {
        cluster.scp(resource("/hive-test.sh"))
        cluster.sshCommand("bash ./hive-test.sh")
      }

      scenario("The user can terminate the cluster") {
        cluster.terminate()
        cluster.state.get must (be ("terminating") or be ("terminated"))
        cluster.ensureState("terminated")
      }
    }
  }
}
