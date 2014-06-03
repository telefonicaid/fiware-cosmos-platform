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

package es.tid.cosmos.api.wizards

import scala.concurrent.{Await, Future}
import scala.concurrent.duration._
import scala.language.postfixOps
import scalaz.Failure

import org.mockito.Matchers.{eq => the, any}
import org.mockito.Mockito.{doReturn, spy}
import org.scalatest.FlatSpec
import org.scalatest.matchers.{Matcher, MustMatchers}
import org.scalatest.mock.MockitoSugar
import org.scalatest.concurrent.Eventually

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers.{registerUser, userIdFor}
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.report.ClusterReporter
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser, UnfilteredPassThrough}
import es.tid.cosmos.servicemanager.clusters.{Running, ClusterId, Terminated}

class UserUnregistrationWizardTest
  extends FlatSpec with MustMatchers with FutureMatchers with Eventually with MockitoSugar {

  val timeout = 1.second
  val failure = new RuntimeException("Forced failure")
  val failedFuture: Future[Unit] = Future.failed(failure)

  trait WithWizard extends MockCosmosDataStoreComponent {
    val sm = spy(new MockedServiceManager())
    val reporter = mock[ClusterReporter]
    val wizard = new UserUnregistrationWizard(store, sm, reporter)

    sm.defineCluster(MockedServiceManager.PersistentHdfsProps)
  }

  trait WithExistingUser extends WithWizard {
    val cosmosProfile = registerUser("jsmith")(store)
    val cosmosId = cosmosProfile.id

    def unregistrationMust(futureMatcher: Matcher[Future[_]]) {
      store.withTransaction { implicit c =>
        wizard.unregisterUser(cosmosId)
      }.fold(
        fail = message => fail(s"Unexpected failure with message $message"),
        succ = unreg_> => unreg_> must (runUnder(timeout) and futureMatcher)
      )
    }

    def databaseUser = store.withTransaction { implicit c =>
      store.profile.lookupByUserId(userIdFor("jsmith"))
    }
  }

  trait WithUserWithCluster extends WithExistingUser {
    val (clusterId, _) = sm.createCluster(
      name = ClusterName("cluster1"),
      size = 6,
      serviceInstances = Set.empty,
      users = Seq.empty,
      preConditions = UnfilteredPassThrough
    )
    sm.withCluster(clusterId) { cluster =>
      cluster.completeProvisioning()
      cluster.immediateTermination()
    }
    store.withTransaction { implicit c =>
      store.cluster.register(clusterId, cosmosProfile.id, ClusterSecret.random())
    }
  }

  trait WithUserOfNotOwnedClusters extends WithExistingUser {
    val (clusterId, _) = sm.createCluster(
      name = ClusterName("cluster1"),
      size = 2,
      serviceInstances = Set.empty,
      users = Seq.empty,
      preConditions = UnfilteredPassThrough
    )
    sm.withCluster(clusterId)(_.completeProvisioning())

    eventually { sm.describeCluster(clusterId).get.state must be (Running) }

    Await.ready(sm.addUser(clusterId, ClusterUser.enabled(
      username = cosmosProfile.handle,
      group = None,
      publicKey = cosmosProfile.keys.head.signature,
      isSudoer = false
    )), timeout)
  }

  "Unregistration" must "not be created when user status cannot be changed" in new WithWizard {
    val userId = 0
    store.throwOnUserStateChangeTo(UserState.Creating)
    wizard.unregisterUser(userId) must
      be (Failure(Message(s"Cannot change user cosmosId=$userId status")))
  }

  it must "mark user as deleted" in new WithExistingUser {
    unregistrationMust(eventuallySucceed)
    databaseUser.get.state must be (UserState.Deleted)
  }

  it must "fail when user cannot be marked as deleted" in new WithExistingUser {
    store.throwOnUserStateChangeTo(UserState.Deleted)
    unregistrationMust(eventuallyFailWith(
      s"Cannot remove user with cosmosId=$cosmosId from the database"))
    databaseUser.get.state must be (UserState.Deleting)
  }

  it must "terminate user clusters" in new WithUserWithCluster {
    unregistrationMust(eventuallySucceed)
    sm.describeCluster(clusterId).get.state must be (Terminated)
  }

  it must "disable user in the clusters he's not owner of" in new WithUserOfNotOwnedClusters {
    unregistrationMust(eventuallySucceed)
    sm.listUsers(clusterId).get must contain(ClusterUser.disabled(
      username = cosmosProfile.handle,
      publicKey = cosmosProfile.keys.head.signature
    ))
  }

  it must "keep user in deleting state if clusters cannot be freed" in
    new WithUserWithCluster {
      doReturn(failedFuture).when(sm).terminateCluster(any[ClusterId])
      unregistrationMust(eventuallyFailWith("Cannot terminate cluster"))
      databaseUser.get.state must be (UserState.Deleting)
    }

  it must "fail if user files in the persistent HDFS cannot be removed" in
    new WithUserWithCluster {
      val persistentHdfsId = sm.persistentHdfsId
      doReturn(failedFuture).when(sm).setUsers(the(persistentHdfsId), any[Seq[ClusterUser]])
      unregistrationMust(eventuallyFailWith("Cannot update persistent HDFS users"))
      databaseUser.get.state must be (UserState.Deleting)
    }
}
