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

package es.tid.cosmos.tests.e2e

import es.tid.cosmos.tests.e2e.cluster.Cluster
import es.tid.cosmos.servicemanager.services.{Oozie, Hive, Hdfs, MapReduce2}

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

    scenario(s"The user can use MapReduce2 on the cluster with Infinity") {
      cluster.scp(resource("/mapreduce2-infinity-test.sh"))
      cluster.sshCommand("bash ./mapreduce2-infinity-test.sh")
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
