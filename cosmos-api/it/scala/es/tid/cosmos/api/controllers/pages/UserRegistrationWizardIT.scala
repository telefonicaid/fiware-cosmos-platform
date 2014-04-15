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
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.report.ClusterReporter
import es.tid.cosmos.api.wizards.UserRegistrationWizard
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.ClusterId

class UserRegistrationWizardIT
  extends FlatSpec with MustMatchers with MockitoSugar with FutureMatchers {

  val testTimeout = 3.seconds
  val handle = "jsmith"
  val userId = userIdFor(handle)
  val registration = registrationFor(handle)

  trait WithUserRegistrationWizard extends MockCosmosDataStoreComponent {
    val sm = spy(new MockedServiceManager())
    val reporter = mock[ClusterReporter]
    val instance = new UserRegistrationWizard(store, sm, reporter)
    sm.defineCluster(MockedServiceManager.PersistentHdfsProps)
  }

  "User registration" must "create a new profile with the input data" in
    new WithUserRegistrationWizard {
      val validationResult = instance.registerUser(userId, registration)

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
      val deletedUser = registerUser("deleted")(store)
      store.withTransaction { implicit c =>
        store.profile.setUserState(deletedUser.id, UserState.Deleted)
      }

      val validationResult = instance.registerUser(userId, registration)

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
