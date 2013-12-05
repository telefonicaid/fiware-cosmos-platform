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

package es.tid.cosmos.servicemanager.ambari

import java.net.URI
import scala.util.Try

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.{MySqlConnDetails, MySqlDatabase}
import es.tid.cosmos.platform.common.scalatest.tags.HasExternalDependencies
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.ClusterUser

class SqlClusterDaoIT extends FlatSpec with MustMatchers with BeforeAndAfter {
  val db = new MySqlDatabase(MySqlConnDetails("localhost", 3306, "root", "", "smtest"))
  val dao = new SqlClusterDao(db)

  before  {
    dao.newTransaction {
      Try {
        SqlClusterDao.drop
      }
      SqlClusterDao.create
    }
  }

  trait ClusterCreated {
    val id = ClusterId()
    dao.registerCluster(id, "cosmos", 3)
    val cluster = dao.getDescription(id).get
  }

  trait ClusterCreatedWithUsers extends ClusterCreated {
    val user1 = ClusterUser("jsmith", "jsmith-public-key")
    dao.setUsers(id, Set(user1))
  }

  "SqlCluster" must "persist a new cluster" taggedAs HasExternalDependencies in new ClusterCreated {
    cluster.name must be ("cosmos")
    cluster.size must be (3)
    cluster.nameNode must be (None)
    cluster.master must be (None)
    cluster.state must be (Provisioning)
    cluster.slaves must be (Seq())
  }

  it must "update a cluster name" taggedAs HasExternalDependencies in new ClusterCreated {
    cluster.name = "Another name"
    cluster.name must be ("Another name")
  }

  it must "update a cluster nameNode URI" taggedAs HasExternalDependencies in new ClusterCreated {
    val uri = new URI("hdfs://host:1234")
    cluster.nameNode = uri
    cluster.nameNode must be (Some(uri))
  }

  it must "update a cluster with a master node" taggedAs HasExternalDependencies in new ClusterCreated {
    val master = HostDetails("Saxon","1.1.1.1")
    cluster.master = master
    cluster.master must be (Some(master))
  }

  it must "update a cluster to Running" taggedAs HasExternalDependencies in new ClusterCreated {
    cluster.state = Running
    cluster.state must be (Running)
  }

  it must "update a cluster to Terminating" taggedAs HasExternalDependencies in new ClusterCreated {
    cluster.state = Terminating
    cluster.state must be (Terminating)
  }

  it must "update a cluster to Terminated" taggedAs HasExternalDependencies in new ClusterCreated {
    cluster.state = Terminated
    cluster.state must be (Terminated)
  }

  it must "update a cluster to Failed" taggedAs HasExternalDependencies in new ClusterCreated {
    cluster.state = Failed("The CPD was nuked")
    cluster.state must be (Failed("The CPD was nuked"))
  }

  it must "update a cluster with a slave nodes" taggedAs HasExternalDependencies in new ClusterCreated {
    val slaves = Seq(HostDetails("Oompa","1.1.1.2"), HostDetails("Lumpa", "1.1.1.3"))
    cluster.slaves = slaves
    cluster.slaves.toSet must be (slaves.toSet)
  }

  it must "return None for non-existing clusters" taggedAs HasExternalDependencies in new ClusterCreated {
    dao.getDescription(ClusterId("IDontExist")) must be (None)
  }

  it must "return all cluster ids" taggedAs HasExternalDependencies in new ClusterCreated {
    val id2 = ClusterId()
    dao.registerCluster(id2, "cosmos", 3)
    dao.ids.toSet must be (Set(id, id2))
  }

  it must "retrieve empty user seq for new cluster" taggedAs HasExternalDependencies in new ClusterCreated {
    dao.getUsers(id) must be (Some(Set.empty))
  }

  it must "retrieve none users for unexisting cluster" taggedAs HasExternalDependencies in new ClusterCreated {
    dao.getUsers(ClusterId()) must not be ('defined)
  }

  it must "set users of existing cluster" taggedAs HasExternalDependencies in new ClusterCreated {
    dao.getUsers(id) must be (Some(Set.empty))
    dao.setUsers(id, Set(ClusterUser("jsmith", "publickey1")))
    dao.getUsers(id) must be (Some(Set(ClusterUser("jsmith", "publickey1"))))
  }

  it must "fail to set users of unexisting cluster" taggedAs HasExternalDependencies in new ClusterCreated {
    evaluating {
      dao.setUsers(ClusterId(), Set(ClusterUser("jsmith", "publickey1")))
    } must produce [IllegalArgumentException]
  }

  it must "replace the users of an existing cluster" taggedAs HasExternalDependencies in new ClusterCreatedWithUsers {
    dao.setUsers(id, Set(ClusterUser("pocahontas", "publickey2")))
    dao.getUsers(id) must be (Some(Set(ClusterUser("pocahontas", "publickey2"))))
  }
}



