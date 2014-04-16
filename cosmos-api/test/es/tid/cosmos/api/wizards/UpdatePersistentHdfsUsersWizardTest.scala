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

import scala.collection.JavaConverters._
import scala.concurrent.{Future, Await}
import scala.concurrent.duration.Duration
import scala.concurrent.ExecutionContext.Implicits.global

import org.mockito.ArgumentCaptor
import org.mockito.Matchers.{eq => the}
import org.mockito.Mockito.{times, verify, spy}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.concurrent.Eventually

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.{UserId, UserState, Registration}
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterUser

class UpdatePersistentHdfsUsersWizardTest
  extends FlatSpec with MustMatchers with FutureMatchers with Eventually {

  trait WithWizard extends MockCosmosDataStoreComponent {
    val sm = spy(new MockedServiceManager())
    sm.deployPersistentHdfsCluster()
    val wizard = new UpdatePersistentHdfsUsersWizard(store, sm)
  }

  "The update users wizard" must "handle concurrent update petitions by serializing them" in
    new WithWizard {
      val numberOfUsers = 10
      val futures = for (i <- 1 to numberOfUsers)
        yield store.withTransaction { implicit c =>
          store.profile.register(
            UserId(s"user$i"),
            Registration(s"user$i", "ssh-rsa pubkey test", s"user$i@test.com"),
            UserState.Enabled)
          wizard.updatePersistentHdfsUsers()
        }
      Await.result(Future.sequence(futures), Duration.Inf)
      val persistentHdfsId = sm.persistentHdfsId
      val userCaptor = ArgumentCaptor.forClass(classOf[Seq[ClusterUser]])
      verify(sm, times(numberOfUsers)).setUsers(the(persistentHdfsId), userCaptor.capture())
      userCaptor.getAllValues.asScala.zipWithIndex.foreach(pair => {
        val userNames = pair._1.map(_.username)
        val index = pair._2
        for (i <- 1 to index) {
          userNames must contain (s"user$i")
        }
      })
    }
}
