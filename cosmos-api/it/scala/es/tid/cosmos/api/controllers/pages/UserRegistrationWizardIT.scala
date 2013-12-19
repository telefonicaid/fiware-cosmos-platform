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

package es.tid.cosmos.api.controllers.pages

import scala.concurrent.duration._

import org.mockito.ArgumentCaptor
import org.mockito.Matchers.any
import org.mockito.Mockito.{verify, spy}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.wizards.UserRegistrationWizard
import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.ClusterId
import scala.concurrent.Await

class UserRegistrationWizardIT
  extends FlatSpec with MustMatchers with MockitoSugar with FutureMatchers {

  val testTimeout = 3.seconds
  val handle = "jsmith"
  val userId = userIdFor(handle)
  val registration = registrationFor(handle)

  trait WithUserRegistrationWizard {
    val dao = new MockCosmosProfileDao()
    val sm = spy(new MockedServiceManager(transitionDelay = 0 milliseconds))
    val instance = new UserRegistrationWizard(sm)

    Await.ready(sm.deployPersistentHdfsCluster(), testTimeout)
  }

  "User registration" must "create a new profile with the input data" in
    new WithUserRegistrationWizard {
      val validationResult = dao.withTransaction { implicit c =>
        instance.registerUser(dao, userId, registration)
      }

      validationResult must be ('success)
      val (profile, registration_>) = validationResult.toOption.get
      registration_> must (runUnder(testTimeout) and eventuallySucceed)

      profile.handle must be (handle)
      profile.email must be (registration.email)
      profile.state must be (UserState.Creating)
      profile.keys must have size 1
    }

  it must "reconfigure persistent HDFS cluster with current and deleted users" in
    new WithUserRegistrationWizard {
      val deletedUser = registerUser("deleted")(dao)
      dao.withTransaction { implicit c =>
        dao.setUserState(deletedUser.id, UserState.Deleted)
      }

      val validationResult = dao.withTransaction { implicit c =>
        instance.registerUser(dao, userId, registration)
      }

      validationResult must be ('success)
      val (_, registration_>) = validationResult.toOption.get
      registration_> must (runUnder(testTimeout) and eventuallySucceed)

      val usersCaptor = ArgumentCaptor.forClass(classOf[Seq[ClusterUser]])
      verify(sm).setUsers(any[ClusterId], usersCaptor.capture())
      val users = usersCaptor.getValue
      users.map(_.username).toSet must be (Set("deleted", handle))
      users.find(_.username == "deleted").get must (not be 'hdfsEnabled and not be 'sshEnabled)
      users.find(_.username == handle).get must (be ('hdfsEnabled) and not be 'sshEnabled)
    }
}
