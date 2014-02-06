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

package es.tid.cosmos.api.wizards

import scala.concurrent.{Await, Future}
import scala.concurrent.duration._
import scalaz.Failure

import org.mockito.Matchers.{eq => the, any}
import org.mockito.Mockito.{doReturn, doThrow, spy}
import org.scalatest.FlatSpec
import org.scalatest.matchers.{Matcher, MustMatchers}
import org.scalatest.concurrent.Eventually

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.UserState
import CosmosProfileTestHelpers.{registerUser, userIdFor}
import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.{ClusterUser, UnfilteredPassThrough}
import es.tid.cosmos.servicemanager.clusters.{ClusterState, Running, ClusterId, Terminated}

class UserUnregistrationWizardTest
  extends FlatSpec with MustMatchers with FutureMatchers with Eventually {

  val timeout = 1.second
  val failure = new RuntimeException("Forced failure")
  val failedFuture: Future[Unit] = Future.failed(failure)

  trait WithWizard {
    val sm = spy(new MockedServiceManager())
    val dao = spy(new MockCosmosProfileDao())
    val wizard = new UserUnregistrationWizard(sm)

    sm.defineCluster(MockedServiceManager.PersistentHdfsProps)
  }

  trait WithExistingUser extends WithWizard {
    val cosmosProfile = registerUser("jsmith")(dao)
    val cosmosId = cosmosProfile.id

    def unregistrationMust(futureMatcher: Matcher[Future[_]]) {
      dao.withTransaction { implicit c =>
        wizard.unregisterUser(dao, cosmosId)
      }.fold(
        fail = message => fail(s"Unexpected failure with message $message"),
        succ = unreg_> => unreg_> must (runUnder(timeout) and futureMatcher)
      )
    }

    def databaseUser = dao.withTransaction { implicit c =>
      dao.lookupByUserId(userIdFor("jsmith"))
    }
  }

  trait WithUserWithCluster extends WithExistingUser {
    val clusterId = sm.createCluster(
      name = "cluster1",
      size = 6,
      serviceDescriptions = Seq.empty,
      users = Seq.empty,
      preConditions = UnfilteredPassThrough
    )
    sm.withCluster(clusterId) { cluster =>
      cluster.completeProvisioning()
      cluster.immediateTermination()
    }
    dao.withTransaction { implicit c =>
      dao.assignCluster(clusterId, cosmosProfile.id)
    }
  }

  trait WithUserOfNotOwnedClusters extends WithExistingUser {
    val clusterId = sm.createCluster(
      name = "cluster1",
      size = 2,
      serviceDescriptions = Seq.empty,
      users = Seq.empty,
      preConditions = UnfilteredPassThrough
    )
    sm.withCluster(clusterId)(_.completeProvisioning())

    eventually { sm.describeCluster(clusterId).get.state must be (Running) }

    Await.ready(sm.addUser(clusterId, ClusterUser.enabled(
      username = cosmosProfile.handle,
      publicKey = cosmosProfile.keys.head.signature,
      isSudoer = false
    )), timeout)
  }

  "Unregistration" must "not be created when user status cannot be changed" in new WithWizard {
    doThrow(failure).when(dao)
      .setUserState(any[Long], any[UserState])(the(MockCosmosProfileDao.DummyConnection))
    val userId = 0
    dao.withTransaction { implicit c =>
      wizard.unregisterUser(dao, userId)
    } must be (Failure(Message(s"Cannot change user cosmosId=$userId status")))
  }

  it must "mark user as deleted" in new WithExistingUser {
    unregistrationMust(eventuallySucceed)
    databaseUser.get.state must be (UserState.Deleted)
  }

  it must "fail when user cannot be mark as deleted" in new WithExistingUser {
    doThrow(failure).when(dao)
      .setUserState(the(cosmosId), the(UserState.Deleted))(the(MockCosmosProfileDao.DummyConnection))
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
