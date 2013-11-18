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

import org.mockito.Matchers.any
import org.mockito.Mockito.{verify, times, spy}
import org.mockito.ArgumentCaptor
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.ClusterId

class UserRegistrationWizardIT extends FlatSpec with MustMatchers with MockitoSugar {

  val email = "jsmith@example.com"
  val registration = Registration("jsmith", "ssh-rsa AAAAAA jsmith@localhost", email)

  trait WithUserRegistrationWizard {
    val dao = new MockCosmosProfileDao()
    val sm = spy(new MockedServiceManager(transitionDelay = 0))
    val instance = new UserRegistrationWizard(dao, sm)
  }

  "User registration" must "create a new profile with the input data" in
    new WithUserRegistrationWizard {
      val email = "jsmith@example.com"
      val profile = instance.registerUser(UserId("id1234"), registration)
      profile.handle must be ("jsmith")
      profile.email must be (email)
      profile.state must be (UserState.Enabled)
      profile.keys must have size 1
    }

  it must "reconfigure persistent HDFS cluster with current and deleted users" in
    new WithUserRegistrationWizard {
      val deletedUser = instance.registerUser(
        UserId("id0001"), registration.copy(handle = "deleted"))
      dao.withTransaction { implicit c =>
        dao.setUserState(deletedUser.id, UserState.Deleted)
      }
      val newUser = instance.registerUser(UserId("id1234"), registration)

      val users = ArgumentCaptor.forClass(classOf[Seq[ClusterUser]])
      verify(sm, times(2)).setUsers(any[ClusterId], users.capture())
      val expectedUsers = Seq(
        ClusterUser(
          userName = deletedUser.handle,
          publicKey = deletedUser.keys(0).signature,
          hdfsEnabled = false,
          sshEnabled = false
        ),
        ClusterUser(
          userName = newUser.handle,
          publicKey = newUser.keys(0).signature,
          hdfsEnabled = true,
          sshEnabled = false
        )
      )
      users.getValue must equal (expectedUsers)
    }
}
