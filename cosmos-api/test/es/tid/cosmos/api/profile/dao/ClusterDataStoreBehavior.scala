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

package es.tid.cosmos.api.profile.dao

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._

trait ClusterDataStoreBehavior extends MustMatchers { this: FlatSpec =>

  def clusterDataStore(withStore: DaoTest => Unit) {
    it must "assign cluster ownership and remember it" in withStore { implicit store =>
      store.withTransaction { implicit c =>
        val clusterId = ClusterId.random()
        val id1 = registerUser(store, "user1").id
        val id2 = registerUser(store, "user2").id
        store.cluster.register(clusterId, id2)
        store.cluster.ownedBy(id1).map(_.clusterId).toList must not contain clusterId
        store.cluster.ownedBy(id2).map(_.clusterId).toList must contain (clusterId)
      }
    }

    it must "retrieve the owner of a cluster" in withStore { implicit store =>
      val clusterId = ClusterId.random()
      val profileId = store.withTransaction { implicit c =>
        val profileId = registerUser(store, "user1").id
        store.cluster.register(clusterId, profileId)
        profileId
      }
      store.withTransaction { implicit c =>
        store.cluster.ownerOf(clusterId) must be (Some(profileId))
      }
    }
  }
}
