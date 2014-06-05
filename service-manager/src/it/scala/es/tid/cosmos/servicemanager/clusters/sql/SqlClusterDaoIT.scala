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

package es.tid.cosmos.servicemanager.clusters.sql

import java.net.URI

import org.mockito.BDDMockito.given
import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.{MySqlDatabase, MySqlConnDetails}
import es.tid.cosmos.common.scalatest.tags.{TaggedTests, HasExternalDependencies}
import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.Service

class SqlClusterDaoIT
  extends FlatSpec with MustMatchers with BeforeAndAfter with TaggedTests with MockitoSugar {

  override val testsTag = HasExternalDependencies

  val db = new MySqlDatabase(MySqlConnDetails("localhost", 3306, "root", "", "smtest"))
  val dao = new SqlClusterDao(db)

  before  {
    dao.newTransaction {
      try {
        ClusterSchema.drop
      } finally {
        ClusterSchema.create
      }
    }
  }

  trait ClusterCreated {
    val id = ClusterId.random()
    val serviceA, serviceB = mock[Service]
    given(serviceA.name).willReturn("serviceA")
    given(serviceB.name).willReturn("serviceB")
    dao.registerCluster(id, ClusterName("cosmos"), 3, Set(serviceA, serviceB))
    val cluster = dao.getDescription(id).get
  }

  trait ClusterCreatedWithUsers extends ClusterCreated {
    val user1 = ClusterUser("jsmith", Some("group"), "jsmith-public-key")
    dao.setUsers(id, Set(user1))
  }

  "SqlCluster" must "persist a new cluster" in new ClusterCreated {
    cluster.name must be ("cosmos")
    cluster.size must be (3)
    cluster.nameNode must be (None)
    cluster.master must be (None)
    cluster.state must be (Provisioning)
    cluster.slaves must be (Seq())
    cluster.services must be (Set("serviceA", "serviceB"))
  }

  it must "update a cluster name" in new ClusterCreated {
    val anotherName = ClusterName("Another name")
    cluster.name = anotherName
    cluster.name must be (anotherName)
  }

  it must "update a cluster nameNode URI" in new ClusterCreated {
    val uri = new URI("hdfs://host:1234")
    cluster.nameNode = uri
    cluster.nameNode must be (Some(uri))
  }

  it must "update a cluster with a master node" in new ClusterCreated {
    val master = HostDetails("Saxon","1.1.1.1")
    cluster.master = master
    cluster.master must be (Some(master))
  }

  it must "update a cluster to Running" in new ClusterCreated {
    cluster.state = Running
    cluster.state must be (Running)
  }

  it must "update a cluster to Terminating" in new ClusterCreated {
    cluster.state = Terminating
    cluster.state must be (Terminating)
  }

  it must "update a cluster to Terminated" in new ClusterCreated {
    cluster.state = Terminated
    cluster.state must be (Terminated)
  }

  it must "update a cluster to Failed" in new ClusterCreated {
    cluster.state = Failed("The CPD was nuked")
    cluster.state must be (Failed("The CPD was nuked"))
  }

  it must "update a cluster's services" in new ClusterCreated {
    cluster.services = Set("ServiceA", "ServiceC")
    cluster.services must be (Set("ServiceA", "ServiceC"))
  }

  it must "update a cluster with a slave nodes" in new ClusterCreated {
    val slaves = Seq(HostDetails("Oompa","1.1.1.2"), HostDetails("Lumpa", "1.1.1.3"))
    cluster.slaves = slaves
    cluster.slaves.toSet must be (slaves.toSet)
  }

  it must "return None for non-existing clusters" in new ClusterCreated {
    dao.getDescription(ClusterId("IDontExist")) must be (None)
  }

  it must "return all cluster ids" in new ClusterCreated {
    val id2 = ClusterId.random()
    dao.registerCluster(id2, ClusterName("cosmos"), 3, Set.empty)
    dao.ids.toSet must be (Set(id, id2))
  }

  it must "retrieve empty user seq for new cluster" in new ClusterCreated {
    dao.getUsers(id) must be (Some(Set.empty))
  }

  it must "retrieve none users for unexisting cluster" in new ClusterCreated {
    dao.getUsers(ClusterId.random()) must not be 'defined
  }

  it must "set users of existing cluster" in new ClusterCreated {
    dao.getUsers(id) must be (Some(Set.empty))
    dao.setUsers(id, Set(ClusterUser("jsmith", Some("group1"), "publickey1")))
    dao.getUsers(id) must be (Some(Set(ClusterUser("jsmith", Some("group1"), "publickey1"))))
  }

  it must "fail to set users of unexisting cluster" in new ClusterCreated {
    evaluating {
      dao.setUsers(ClusterId.random(), Set(ClusterUser("jsmith", Some("group1"), "publickey1")))
    } must produce [IllegalArgumentException]
  }

  it must "replace the users of an existing cluster" in new ClusterCreatedWithUsers {
    dao.setUsers(id, Set(ClusterUser("pocahontas", Some("group2"), "publickey2")))
    dao.getUsers(id) must be (Some(Set(ClusterUser("pocahontas", Some("group2"), "publickey2"))))
  }
}
