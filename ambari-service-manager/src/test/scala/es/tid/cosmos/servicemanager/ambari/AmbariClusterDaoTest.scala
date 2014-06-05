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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.Future

import com.ning.http.client.Request
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito.given
import org.mockito.Matchers.any

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.ambari.AmbariClusterState.AmbariClusterState
import es.tid.cosmos.servicemanager.ambari.clusters.InMemoryClusterDao
import es.tid.cosmos.servicemanager.ambari.rest.{Cluster, AmbariServer}
import es.tid.cosmos.servicemanager.ambari.services.AmbariService
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.{Service, Hdfs, MapReduce2}

class AmbariClusterDaoTest extends FlatSpec with MustMatchers with MockitoSugar {

  class MockClusterStateResolver(returnState: AmbariClusterState) {
    trait Trait extends ClusterStateResolver {
      override def resolveState(cluster: Cluster, allServices: Set[AmbariService]) =
        Future.successful(returnState)
    }
  }

  trait WithMockCluster {
    val innerDao = new InMemoryClusterDao
    val ambariServer = mock[AmbariServer]
    val services = Set.empty[AmbariService]
    val id = ClusterId.random()
    val enabledServices: Set[Service] = Set(Hdfs, MapReduce2)
    val clusterDesc = innerDao.registerCluster(id, ClusterName("test"), 5, enabledServices)
    clusterDesc.state = Running
    val ambariCluster = mock[Cluster]
    val ignoredRequest = mock[Request]
    given(ambariServer.getCluster(any[String])).willReturn(Future.successful(ambariCluster))
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq()))
  }

  "The AmbariClusterDao" must "set to Terminated a cluster found in dao but not in ambari" in
      new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)),
      Future.successful(Seq()))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.ClusterNotPresent)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Terminated)
  }

  it must "set to Running a cluster found Terminating in dao but Running in ambari" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)))
    clusterDesc.state = Terminating
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Running)
  }

  it must "set to Failed a cluster found Running in dao but Unknown in ambari" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Unknown)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Failed("Mismatch between SQL and Ambari information"))
  }

  it must "set to Running a cluster found Provisioning in dao but Running in ambari" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString)))
    clusterDesc.state = Provisioning
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getDescription(id).get.state must be (Running)
  }

  it must "set to Terminated every cluster found in dao but not in ambari" in new WithMockCluster {
    val id2 = ClusterId.random()
    val id3 = ClusterId.random()
    val cl2 = innerDao.registerCluster(id2, ClusterName("cluster2"), 2, Set())
    val cl3 = innerDao.registerCluster(id3, ClusterName("cluster3"), 3, Set())
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq(id.toString,
      id2.toString)))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.ids
    clusterDesc.state must not be Terminated
    cl2.state must not be Terminated
    cl3.state must be (Terminated)
  }

  it must "pass through register requests" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq()))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    val newId = ClusterId.random()
    val newCluster = dao.registerCluster(newId, ClusterName("newCluster"), 10, Set())
    innerDao.getDescription(newId).get must be (newCluster)
  }

  it must "pass through getUser requests" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq()))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    dao.getUsers(id) must be (innerDao.getUsers(id))
  }

  it must "pass through setUser requests" in new WithMockCluster {
    given(ambariServer.listClusterNames).willReturn(Future.successful(Seq()))
    val mockResolver = new MockClusterStateResolver(AmbariClusterState.Running)
    val dao = new AmbariClusterDao(innerDao, ambariServer, services) with mockResolver.Trait
    val newUsers = Set(ClusterUser.enabled("foo", Some("group"), "pK", false))
    dao.setUsers(id, newUsers)
    innerDao.getUsers(id) must be (Some(newUsers))
  }
}
