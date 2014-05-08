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

package es.tid.cosmos.api.controllers.infinity

import scalaz._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.quota.{Quota, GuaranteedGroup}
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters._

class InfinityAuthenticatorTest extends FlatSpec with MustMatchers {

  import Scalaz._

  trait WithAuthentication extends MockCosmosDataStoreComponent {
    val serviceManager = new MockedServiceManager()
    val instance = new InfinityAuthenticator(store, serviceManager)
    val profile = CosmosProfileTestHelpers.registerUser("user")(store)
    val clusterId = ClusterId.random()

    def createCluster(targetState: ClusterState = Running): ImmutableClusterDescription = {
      val cluster = serviceManager.defineCluster(ClusterProperties(
        id = clusterId,
        name = ClusterName("my private cluster"),
        size = 2,
        users = Set.empty
      ))
      cluster.setState(targetState)
      cluster.view
    }

    def registerCluster(shared: Boolean = false) = store.withTransaction { implicit c =>
      store.cluster.register(clusterId, profile.id, ClusterSecret.random(), shared)
    }.secret.get
  }

  "API key and secret authentication" must "be accepted with 777 mask and for any host" in
    new WithAuthentication {
      instance.authenticateApiCredentials(profile.apiCredentials) must be (InfinityIdentity(
        user = "user",
        groups = Seq("No Group"),
        accessMask = AccessMask("777")
      ).success)
    }

  it must "be rejected if credentials are unknown" in new WithAuthentication {
    val unknownCredentials = ApiCredentials.random()
    instance.authenticateApiCredentials(unknownCredentials) must
      be (Message("Unknown API credentials").failure)
  }

  it must "be rejected if the user profile is not enabled" in new WithAuthentication {
    store.withTransaction { implicit c =>
      store.profile.setUserState(profile.id, UserState.Disabled)
    }
    instance.authenticateApiCredentials(profile.apiCredentials) must
      be (Message("User is not enabled").failure)
  }

  it must "reflect the current group of the user" in new WithAuthentication {
    val group = GuaranteedGroup("myGroup", Quota(5))
    store.withTransaction { implicit c =>
      store.group.register(group)
      store.profile.setGroup(profile.id, group)
    }
    instance.authenticateApiCredentials(profile.apiCredentials).map(_.groups) must
      be (Seq("myGroup").success)
  }

  "Cluster secret authentication" must "be accepted with 777 mask and just for the cluster hosts" in
    new WithAuthentication {
      val cluster = createCluster()
      val clusterSecret = registerCluster()
      val result = instance.authenticateClusterSecret(clusterSecret)
      result.map(_.accessMask) must be (AccessMask("777").success)
      result.map(_.origins) must be (Some(cluster.hosts.map(_.ipAddress)).success)
    }

  it must "be rejected if cluster is defined but not yet exists" in new WithAuthentication {
    val clusterSecret = registerCluster()
    instance.authenticateClusterSecret(clusterSecret) must
      be (Message("Cluster does not yet exist").failure)
  }

  it must "be rejected if cluster is not in running state" in new WithAuthentication {
    createCluster(targetState = Terminated)
    val clusterSecret = registerCluster()
    instance.authenticateClusterSecret(clusterSecret) must
      be (Message("Cluster is not in running state").failure)
  }

  it must "be accepted with 077 mask for shared clusters" in new WithAuthentication {
    val cluster = createCluster()
    val clusterSecret = registerCluster(shared = true)
    instance.authenticateClusterSecret(clusterSecret).map(_.accessMask) must
      be (AccessMask("077").success)
  }
}
