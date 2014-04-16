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

import scala.collection.JavaConverters._
import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration.Duration

import org.mockito.ArgumentCaptor
import org.mockito.Matchers.{eq => the}
import org.mockito.Mockito.{spy, times, verify}
import org.scalatest.FlatSpec
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.{Registration, UserId, UserState}
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

class UpdatePersistentHdfsUsersWizardTest
  extends FlatSpec with MustMatchers with FutureMatchers with Eventually {

  trait WithWizard extends MockCosmosDataStoreComponent {
    val sm = spy(new MockedServiceManager())
    sm.deployPersistentHdfsCluster(InfinityServerParameters("http://cosmos-api", "secret"))
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
