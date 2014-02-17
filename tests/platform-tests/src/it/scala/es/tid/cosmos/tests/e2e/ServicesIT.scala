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

import es.tid.cosmos.servicemanager.ambari.services.{Oozie, Hdfs, MapReduce2, Hive}
import es.tid.cosmos.tests.e2e.cluster.Cluster

class ServicesIT extends E2ETestBase {

  val services = Seq(Hive, Oozie, MapReduce2, Hdfs).map(_.name)

  withNewUser { user =>
    var cluster: Cluster = null

    scenario(s"The user can request a cluster with $services") {
      cluster = Cluster(2, user, services)
      cluster.isListed() must be (true)
      cluster.state().get must (be ("provisioning") or be ("running"))
      cluster.ensureState("running")
    }

    scenario(s"The user can use HDFS on the cluster") {
      cluster.scp(resource("/hdfs-test.sh"))
      cluster.sshCommand("bash ./hdfs-test.sh")
    }

    scenario(s"The user can use MapReduce2 on the cluster") {
      cluster.scp(resource("/mapreduce2-test.sh"))
      cluster.sshCommand("bash ./mapreduce2-test.sh")
    }

    scenario(s"The user can use Hive on the cluster") {
      cluster.scp(resource("/hive-test.sh"))
      cluster.sshCommand("bash ./hive-test.sh")
    }

    scenario("The user can use Oozie on the cluster") {
      cluster.scp(resource("/oozie-test.sh"))
      cluster.sshCommand("bash ./oozie-test.sh")
    }

    scenario("The user can terminate the cluster") {
      cluster.terminate()
      cluster.state().get must (be ("terminating") or be ("terminated"))
      cluster.ensureState("terminated")
    }
  }
}
