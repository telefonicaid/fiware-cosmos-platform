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

import play.core.DevSettings
import play.api.mvc.Session
import play.api.test.{FakeRequest, WithApplication, FakeApplication}

import es.tid.cosmos.api.AbstractGlobal
import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.{Registration, CosmosProfileDao}

class WithTestApplication(
    additionalConfiguration: Map[String, String] = Map.empty,
    val playGlobal: AbstractGlobal = new TestGlobal
  ) extends WithApplication(WithTestApplication.buildApp(additionalConfiguration, playGlobal)) {

  lazy val dao = playGlobal.application.dao

  def services = playGlobal.application.services

  def registerUser(dao: CosmosProfileDao, user: OAuthUserProfile): CosmosProfile =
    dao.withConnection { implicit c =>
      val OAuthUserProfile(authId, _, email) = user
      val handle = email.map(_.split('@')(0)).getOrElse("root")
      dao.registerUserInDatabase(authId, Registration(handle, "pk1234"))
    }

  def withSession[A](request: FakeRequest[A], session: Session) =
    request.withSession(session.data.toSeq: _*)
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
