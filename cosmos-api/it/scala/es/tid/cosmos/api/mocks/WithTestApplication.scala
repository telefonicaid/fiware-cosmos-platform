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

package es.tid.cosmos.api.mocks

import play.core.DevSettings
import play.api.mvc.Session
import play.api.test.{FakeApplication, FakeRequest, WithApplication}

import es.tid.cosmos.api.AbstractGlobal
import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.servicemanager.clusters.Running

class WithTestApplication(
    additionalConfiguration: Map[String, String] = Map.empty,
    val testApp: TestApplication = new MockDaoTestApplication)
  extends WithApplication(
    WithTestApplication.buildApp(additionalConfiguration, testApp.global)) {

  lazy val playGlobal = testApp.global

  val mockedServiceManager = testApp.mockedServiceManager

  lazy val store = playGlobal.application.store

  def services = playGlobal.application.services

  /** Register a user for testing purposes (directly enabled, no infinity registration)
    *
    * @param user  OAuth data to fill in the registration data
    * @return      Newly created Cosmos profile
    */
  def registerUser(user: OAuthUserProfile): CosmosProfile =
    store.withConnection { implicit c =>
      val email = user.email.getOrElse("root@host")
      val handle = email.split('@')(0)
      val reg = Registration(handle, s"ssh-rsa ABCDE $email", email)
      store.profile.register(user.id, reg, UserState.Enabled)
    }

  def withSession[A](request: FakeRequest[A], session: Session) =
    request.withSession(session.data.toSeq: _*)

  def withPersistentHdfsDeployed(action: => Unit) = {
    mockedServiceManager.defineCluster(
      MockedServiceManager.PersistentHdfsProps.copy(initialState = Some(Running)))
    action
  }
}

object WithTestApplication {

  private def buildApp(additionalConfiguration: Map[String, String], global: AbstractGlobal) =
    new FakeApplication(
      additionalConfiguration = additionalConfiguration,
      withGlobal = Some(global)
    ) with DevSettings {
      lazy val devSettings = Map("config.file" -> "cosmos-api/it/resources/application.conf")
    }
}
