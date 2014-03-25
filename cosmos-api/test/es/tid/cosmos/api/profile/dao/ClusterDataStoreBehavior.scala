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

import java.util.Date
import java.sql.Timestamp

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.{ClusterSecret, Cluster}
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait ClusterDataStoreBehavior extends MustMatchers { this: FlatSpec =>

  type ClusterTest = ClusterDataStore with ProfileDataStore => Unit

  def clusterDataStore(withStore: ClusterTest => Unit) {

    it must "assign cluster ownership and remember it" in withStore { store =>
      store.withTransaction { implicit c =>
        val clusterId = ClusterId.random()
        val id1 = registerUser(store, "user1").id
        val id2 = registerUser(store, "user2").id
        store.cluster.register(clusterId, id2, ClusterSecret.random())
        store.cluster.ownedBy(id1).map(_.clusterId).toList must not contain clusterId
        store.cluster.ownedBy(id2).map(_.clusterId).toList must contain (clusterId)
      }
    }

    it must "retrieve the owner of a cluster" in withStore { store =>
      val clusterId = ClusterId.random()
      val profileId = store.withTransaction { implicit c =>
        val profileId = registerUser(store, "user1").id
        store.cluster.register(clusterId, profileId, ClusterSecret.random())
        profileId
      }
      store.withTransaction { implicit c =>
        store.cluster.ownerOf(clusterId) must be (Some(profileId))
      }
    }

    it must "retrieve a cluster by its secret" in withStore { store =>
      val secret = ClusterSecret("X" * ClusterSecret.Length)
      store.withTransaction { implicit c =>
        val owner = registerUser(store, "handle")
        val cluster = Cluster(
          clusterId = ClusterId("mycluster"),
          ownerId = owner.id,
          creationDate = timestampWithSecondsPrecision(),
          secret = Some(secret)
        )
        store.cluster.register(cluster)

        store.cluster.lookupBySecret(secret) must be (Some(cluster))
        store.cluster.lookupBySecret(ClusterSecret.random()) must not be 'defined
      }
    }
  }

  private def timestampWithSecondsPrecision(): Timestamp =
    new Timestamp(new Date().getTime / 1000 * 1000)
}
