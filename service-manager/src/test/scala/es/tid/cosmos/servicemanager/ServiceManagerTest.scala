/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager

import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.clusters.{Running, ImmutableClusterDescription, ClusterId}
import es.tid.cosmos.servicemanager.services.Service
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

class ServiceManagerTest extends FlatSpec with MustMatchers with FutureMatchers {


  "A service manager" must "provide a cluster description when future succeeds" in {
    val sm = new TestServiceManager(Some(description))
    val description_> = sm.describeClusterUponCompletion(clusterId, Future.successful())
    description_> must eventually (be (Some(description)))
  }

  it must "provide a cluster description when future succeeds even if description is unavailable" in {
      val sm = new TestServiceManager(None)
      val description_> = sm.describeClusterUponCompletion(clusterId, Future.successful())
      description_> must eventually (be (None))
    }

  it must "provide a cluster description when future fails and description exists" in {
    val sm = new TestServiceManager(Some(description))
    val description_> = sm.describeClusterUponCompletion(
      clusterId, Future.failed(new RuntimeException("Oops")))
    description_> must eventually (be (Some(description)))
  }

  it must "pass the future failure through when future fails and no description is available" in {
    val sm = new TestServiceManager(None)
    val description_> = sm.describeClusterUponCompletion(
      clusterId, Future.failed(new RuntimeException("Oops")))
    description_> must eventuallyFailWith[RuntimeException]
  }

  val clusterId = ClusterId("AnID")
  val description = ImmutableClusterDescription(
    id = clusterId,
    name = ClusterName("aCluster"),
    size = 2,
    state = Running,
    nameNode = None,
    master = None,
    slaves = Seq.empty,
    users = None,
    services = Set.empty,
    blockedPorts = Set.empty
  )

  class TestServiceManager(description: Option[ImmutableClusterDescription]) extends ServiceManager {

    override def clusterIds: Seq[ClusterId] = ???
    override def persistentHdfsId: ClusterId = ???
    override def createCluster(
        name: ClusterName,
        clusterSize: Int,
        services: Set[AnyServiceInstance],
        users: Seq[ClusterUser],
        preConditions: ClusterExecutableValidation
      ): (ClusterId, Future[Unit]) = ???
    override def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] = ???
    override def clusterNodePoolCount: Int = ???
    override def terminateCluster(id: ClusterId): Future[Unit] = ???
    override def deployPersistentHdfsCluster(parameters: InfinityServerParameters): Future[Unit] = ???
    override def listUsers(clusterId: ClusterId): Option[Seq[ClusterUser]] = ???
    override val optionalServices: Set[Service] = Set.empty

    override def describeCluster(id: ClusterId): Option[ImmutableClusterDescription] = description
  }
}
