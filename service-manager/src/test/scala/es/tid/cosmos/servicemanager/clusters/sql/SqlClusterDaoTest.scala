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
package es.tid.cosmos.servicemanager.clusters.sql

import scala.concurrent.Future

import org.scalatest.{BeforeAndAfterAll, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.servicemanager.services.{Hdfs, InfinityServer, MapReduce2, Service}

class SqlClusterDaoTest extends FlatSpec with MustMatchers with BeforeAndAfterAll {

  val db = new H2InMemoryDatabase

  override protected def beforeAll(): Unit = {
    db.newSession.bindToCurrentThread
    ClusterSchema.create
  }

  "SqlClusterDao" must "allow update the cluster services list" in {
    val dao = new SqlClusterDao(db)
    val clusterId = ClusterId.random()
    val originalServices: Set[Service] = Set(Hdfs, MapReduce2)
    val newServices = (originalServices + InfinityServer).map{_.name}

    dao.registerCluster(clusterId, ClusterName("myCluster"), 1, originalServices)

    val cluster = dao.getDescription(clusterId).get
    cluster.withFailsafe {
      val diffServs =  newServices diff cluster.services
      cluster.services = cluster.services ++ diffServs
      Future.successful(())
    }

    dao.getDescription(clusterId).get.services must be (newServices)
  }

}
