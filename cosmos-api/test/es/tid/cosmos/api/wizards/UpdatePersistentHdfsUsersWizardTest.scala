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
import scala.concurrent.{Future, Await}
import scala.concurrent.duration.Duration
import scala.concurrent.ExecutionContext.Implicits.global

import org.mockito.ArgumentCaptor
import org.mockito.Matchers.{eq => the}
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.concurrent.Eventually

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterUser

class UpdatePersistentHdfsUsersWizardTest
  extends FlatSpec with MustMatchers with FutureMatchers with Eventually {

  val sm = spy(new MockedServiceManager())
  sm.deployPersistentHdfsCluster()
  val dao = spy(new MockCosmosProfileDao())
  val wizard = new UpdatePersistentHdfsUsersWizard(sm)

  "The UpdatePersistentHdfsUsersWizard" must "handle concurrent update petitions by serializing them" in {
    val numberOfUsers = 10
    val futures = for (i <- 1 to numberOfUsers)
      yield dao.withTransaction { implicit c =>
        dao.registerUser(
          UserId(s"user$i"),
          Registration(s"user$i", "ssh-rsa pubkey test", s"user$i@test.com"),
          UserState.Enabled)
        wizard.updatePersistentHdfsUsers(dao)
      }
    Await.result(Future.sequence(futures), Duration.Inf)
    val persistentHdfsId = sm.persistentHdfsId
    val userCaptor = ArgumentCaptor.forClass(classOf[Seq[ClusterUser]])
    verify(sm, times(numberOfUsers)).setUsers(the(persistentHdfsId), userCaptor.capture())
    userCaptor.getAllValues.asScala.zipWithIndex.foreach(pair => {
      val usernames = pair._1.map(_.username)
      val index = pair._2
      for (i <- 1 to index) {
        usernames must contain (s"user$i")
      }
    })
  }
}
