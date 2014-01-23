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

package es.tid.cosmos.api.mocks

import scala.Some
import scala.concurrent.Await
import scala.concurrent.duration._

import play.core.DevSettings
import play.api.mvc.Session
import play.api.test.{FakeApplication, FakeRequest, WithApplication}

import es.tid.cosmos.api.AbstractGlobal
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Registration
import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile

class WithTestApplication(
    additionalConfiguration: Map[String, String] = Map.empty,
    val testApp: TestApplication = new MockDaoTestApplication)
  extends WithApplication(
    WithTestApplication.buildApp(additionalConfiguration, testApp.global)) {

  val playGlobal = testApp.global

  val mockedServiceManager = testApp.mockedServiceManager

  lazy val dao = playGlobal.application.dao

  def services = playGlobal.application.services

  /** Register a user for testing purposes (directly enabled, no infinity registration)
    *
    * @param dao   Where to register the user
    * @param user  OAuth data to fill in the registration data
    * @return      Newly created Cosmos profile
    */
  def registerUser(dao: CosmosProfileDao, user: OAuthUserProfile): CosmosProfile =
    dao.withConnection { implicit c =>
      val email = user.email.getOrElse("root@host")
      val handle = email.split('@')(0)
      val reg = Registration(handle, s"ssh-rsa ABCDE $email", email)
      dao.registerUser(user.id, reg, UserState.Enabled)
    }

  def withSession[A](request: FakeRequest[A], session: Session) =
    request.withSession(session.data.toSeq: _*)

  def withPersistentHdfsDeployed(action: => Unit) = {
    Await.ready(services.serviceManager().deployPersistentHdfsCluster(), 5 seconds)
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
