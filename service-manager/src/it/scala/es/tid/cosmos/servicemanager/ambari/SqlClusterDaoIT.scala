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

class SqlClusterDaoIT extends FlatSpec with MustMatchers with BeforeAndAfter {
  val db = new MySqlDatabase(MySqlConnDetails("localhost", 3306, "root", "", "smtest"))
  val dao = new SqlClusterDao(db)

  before  {
    dao.newTransaction {
      Try {
        dao.drop
      }
      dao.create
    }
  }

  trait ClusterCreated {
    val id = ClusterId()
    dao.registerCluster(id, "cosmos", 3)
    val cluster = dao.getDescription(id).get
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
}



