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

import java.io.File._
import scala.util.Random
import scala.sys.process._

class InfinityServerIT extends E2ETestBase {
  withNewUsers(3) { users =>
    val user1 = users(0)
    val user2 = users(1)
    val user3 = users(2)

    ignore("Setup") {

      forceUserCreation(users)

      // Assign user1 and user2 to the same group
      val cosmosMaster = testConfig.getString("cosmosMaster")
      val groupName = "InfinityServerTest" + Random.nextInt()
      s"ssh $cosmosMaster '/opt/pdi-cosmos/cosmos-admin/cosmos-admin group create -n $groupName'".! must be (0)
      s"ssh $cosmosMaster '/opt/pdi-cosmos/cosmos-admin/cosmos-admin profile set-group -h ${user1.handle} -g $groupName'".! must be (0)
      s"ssh $cosmosMaster '/opt/pdi-cosmos/cosmos-admin/cosmos-admin profile set-group -h ${user2.handle} -g $groupName'".! must be (0)

      // Create files needed for testing
      val source = {
        val file = createTempFile("test", "txt")
        file.deleteOnExit()
        file.getAbsolutePath
      }
      val infinity = new PersistentHdfs(user1)
      infinity.chmod("777", "/")

      infinity.put(source, "onlyUser.txt")
      infinity.chmod("700", "onlyUser.txt")

      infinity.put(source, "onlyGroup.txt")
      infinity.chmod("770", "onlyGroup.txt")

      infinity.put(source, "everyone.txt")
      infinity.chmod("777", "everyone.txt")
    }

    withNewCluster(1, user1) { cluster =>
      ignore("Users can manage their own files") {
        cluster.scp(resource("/infinity-server-unshared-cluster.sh"))
        cluster.sshCommand("bash ./infinity-server-unshared-cluster.sh")
      }
    }

    withNewCluster(1, user1, shared = true) { cluster =>
      ignore("Users can manage their group files in a shared cluster") {
        cluster.scp(resource("/infinity-server-shared-cluster.sh"))
        cluster.sshCommand("bash ./infinity-server-shared-cluster.sh")
        cluster.scp(resource("/infinity-server-shared-cluster.sh"), executedBy = user2)
        cluster.sshCommand(s"TARGET_USER=${user1.handle}; bash ./infinity-server-shared-cluster.sh", executedBy = user2)
      }
    }

    withNewCluster(1, user3, shared = true) { cluster =>
      ignore("Users can only access files with 'other' permissions if they don't own and aren't part of the group") {
        cluster.scp(resource("/infinity-server-unrelated-user.sh"))
        cluster.sshCommand(s"TARGET_USER=${user1.handle}; bash ./infinity-server-shared-test.sh")
      }
    }
  }

  private def forceUserCreation(users: Seq[LazyVal[User]]): Unit = users.foreach(_.value)
}
