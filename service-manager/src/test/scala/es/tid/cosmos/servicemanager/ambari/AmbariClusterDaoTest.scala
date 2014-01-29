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

import scala.concurrent.Future

import com.ning.http.client.Request
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.mockito.Mockito.{verify, never}

import es.tid.cosmos.servicemanager.ambari.AmbariClusterState.AmbariClusterState
import es.tid.cosmos.servicemanager.ambari.rest.{Cluster, AmbariServer}
import es.tid.cosmos.servicemanager.ambari.services.{MapReduce2, Hdfs, AmbariServiceDescription}
import es.tid.cosmos.servicemanager.clusters._

class AmbariClusterDaoTest extends FlatSpec with MustMatchers with MockitoSugar {

  class MockClusterStateResolver(returnState: AmbariClusterState) {
    trait Trait extends ClusterStateResolver {
      override def resolveState(cluster: Cluster, allServices: Seq[AmbariServiceDescription]) =
        Future.successful(returnState)
    }
  }

  trait WithMockCluster {
    val innerDao = mock[ClusterDao]
    val ambariServer = mock[AmbariServer]
    val services = Seq()
    val id = ClusterId()
    val enabledServices = Set(Hdfs, MapReduce2).map(_.name)
    val clusterDesc = new InMemoryClusterDescription(ClusterId(), "test", 5, enabledServices)
    clusterDesc.state = Running
    val ambariCluster = mock[Cluster]
    val ignoredRequest = mock[Request]
    given(innerDao.getDescription(any[ClusterId])).willReturn(Some(clusterDesc))
    given(innerDao.ids).willReturn(Seq())
    given(ambariServer.getCluster(any[String])).willReturn(Future.successful(ambariCluster))
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq()))
  }

  "The AmbariClusterDao" must "set to Terminated a cluster found in dao but not in ambari" in
      new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)),
      Future.successful(Seq()))
    given(innerDao.ids).willReturn(Seq(id))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.ClusterNotPresent)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Terminated)
  }

  it must "set to Running a cluster found Terminating in dao but Running in ambari" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)))
    given(innerDao.ids).willReturn(Seq(id))
    clusterDesc.state = Terminating
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Running)
  }

  it must "set to Failed a cluster found Running in dao but Unknown in ambari" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)))
    given(innerDao.ids).willReturn(Seq(id))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Unknown)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Failed("Mismatch between SQL and Ambari information"))
  }

  it must "set to Running a cluster found Provisioning in dao but Running in ambari" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)))
    given(innerDao.ids).willReturn(Seq(id))
    clusterDesc.state = Provisioning
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Running)
  }

  it must "set to Terminated every cluster found in dao but not in ambari" in new WithMockCluster {
    val id1 = ClusterId()
    val id2 = ClusterId()
    val id3 = ClusterId()
    val cl1 = mock[MutableClusterDescription]
    val cl2 = mock[MutableClusterDescription]
    val cl3 = mock[MutableClusterDescription]
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id1.toString,
      id2.toString)))
    given(innerDao.ids).willReturn(Seq(id1, id2, id3))
    given(innerDao.getDescription(id1)).willReturn(Some(cl1))
    given(innerDao.getDescription(id2)).willReturn(Some(cl2))
    given(innerDao.getDescription(id3)).willReturn(Some(cl3))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.ids
    verify(cl1, never).state_=(Terminated)
    verify(cl2, never).state_=(Terminated)
    verify(cl3).state_=(Terminated)
  }
}
