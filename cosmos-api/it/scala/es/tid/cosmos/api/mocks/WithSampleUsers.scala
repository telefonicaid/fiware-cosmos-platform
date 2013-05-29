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

import anorm._
import org.specs2.execute.{Result, AsResult}
import play.api.db.DB
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.common.BasicAuth
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.authorization.ApiCredentials

class WithSampleUsers extends WithInMemoryDatabase {

  lazy val user1 = CosmosProfile(1, "user1", ApiCredentials(
    "7qGr9DNvfsh0tQbGofs7DNROLdce7zQurd1B6pHV", "QvD79TLRlNRZ20JaVi6BevN5SdBLt0T8hL29tugo"))
  lazy val user2 = CosmosProfile(2, "user2", ApiCredentials(
    "AAAAB3NzaC1yc2EAAAABIAAAAB3NzaC1yc2EAAAA", "cUN1cxN4x4XYIqVIeYEAYQUKQExd0HNWzd4Y8DiD"))

  override def around[T: AsResult](t: => T): Result = {
    super.around {
      DB.withConnection { implicit c =>
        SQL("""
              | INSERT INTO user(cosmos_id, user_id, handle, api_key, api_secret)
              | VALUES (1, 'tu1', 'user1', {key1}, {secret1}),
              |        (2, 'tu2', 'user2', {key2}, {secret2});
              |
              | INSERT INTO public_key(cosmos_id, name, signature)
              | VALUES (1, 'default', 'ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEA9L'),
              |        (2, 'default', 'ssh-rsa eYEAYQUKQE+xd0HNWz+d4+Y8Di');
              |
            """.stripMargin)
          .on(
            "key1" -> user1.apiCredentials.apiKey,
            "secret1" -> user1.apiCredentials.apiSecret,
            "key2" -> user2.apiCredentials.apiKey,
            "secret2" -> user2.apiCredentials.apiSecret
          )
          .execute()
      }
      t
    }
  }

  implicit def authRequestBuilder[T](r: FakeRequest[T]) = new AuthRequest(r)

  class AuthRequest[T](r: FakeRequest[T]) {
    def authorizedBy(user: CosmosProfile) = r.withHeaders(
      "Authorization" -> BasicAuth(user.apiCredentials.apiKey, user.apiCredentials.apiSecret))
  }
}
