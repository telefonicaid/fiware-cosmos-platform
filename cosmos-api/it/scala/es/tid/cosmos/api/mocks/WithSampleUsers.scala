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

import org.specs2.execute.{Result, AsResult}
import play.api.test.FakeRequest

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.controllers.common.BasicAuth
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.{Registration, UserId}

class WithSampleUsers(additionalConfiguration: Map[String, String] = Map.empty)
  extends WithTestApplication(additionalConfiguration) {

  override def around[T: AsResult](t: => T): Result = {
    super.around {
      dao.withConnection { implicit c =>
        dao.registerUserInDatabase(
          UserId("tu1"), Registration("user1", "ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEA9L"))
        dao.registerUserInDatabase(
          UserId("tu2"), Registration("user2", "ssh-rsa eYEAYQUKQE+xd0HNWz+d4+Y8Di"))
        dao.registerUserInDatabase(
          UserId("tu3"), Registration("user3", "ssh-rsa eYEAYQUKQE+xd0HNWz+d4+Y8Di"))
      }
      t
    }
  }

  def user1 = user("tu1")
  def user2 = user("tu2")
  def user3 = user("tu3")
  private def user(userId: String) = dao.withConnection { implicit c =>
    dao.lookupByUserId(UserId(userId)).get
  }

  implicit def authRequestBuilder[T](r: FakeRequest[T]) = new AuthRequest(r)

  class AuthRequest[T](r: FakeRequest[T]) {

    def authorizedBy(user: CosmosProfile): FakeRequest[T] = authorizedBy(user.apiCredentials)

    def authorizedBy(credentials: ApiCredentials) = r.withHeaders(
      "Authorization" -> BasicAuth(credentials.apiKey, credentials.apiSecret))
  }
}
