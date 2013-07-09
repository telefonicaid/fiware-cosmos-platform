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
import es.tid.cosmos.api.profile.{EmptyQuota, FiniteQuota, UnlimitedQuota}

class WithSampleUsers extends WithInMemoryDatabase {

  lazy val user1 = CosmosProfile(1, "user1", UnlimitedQuota, ApiCredentials(
    "7qGr9DNvfsh0tQbGofs7", "QvD79TLRlNRZ20JaVi6BevN5SdBLt0T8hL29tugo"))
  lazy val user2 = CosmosProfile(2, "user2", FiniteQuota(15), ApiCredentials(
    "IAAAAB3NzaC1yc2EAAAA", "cUN1cxN4x4XYIqVIeYEAYQUKQExd0HNWzd4Y8DiD"))
  lazy val user3 = CosmosProfile(3, "user3", EmptyQuota, ApiCredentials(
    "IAAAAB3NzaC1yc2EBBBB", "cUN1cxN4x4XYIqVIeYEAYQUKQExd0HNWzd4Y8DiD"))

  override def around[T: AsResult](t: => T): Result = {
    super.around {
      DB.withConnection { implicit c =>
        SQL("""
              | INSERT INTO user(cosmos_id, user_id, handle, api_key, api_secret, machine_quota)
              | VALUES (1, 'tu1', 'user1', {key1}, {secret1}, NULL),
              |        (2, 'tu2', 'user2', {key2}, {secret2}, 15),
              |        (3, 'tu3', 'user3', {key3}, {secret3}, 0);
              |
              | INSERT INTO public_key(cosmos_id, name, signature)
              | VALUES (1, 'default', 'ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEA9L'),
              |        (2, 'default', 'ssh-rsa eYEAYQUKQE+xd0HNWz+d4+Y8Di'),
              |        (3, 'default', 'ssh-rsa eYEAYQUKQE+xd0HNWz+d4+Y8Di');
              |
            """.stripMargin)
          .on(
            "key1" -> user1.apiCredentials.apiKey,
            "secret1" -> user1.apiCredentials.apiSecret,
            "key2" -> user2.apiCredentials.apiKey,
            "secret2" -> user2.apiCredentials.apiSecret,
            "key3" -> user3.apiCredentials.apiKey,
            "secret3" -> user3.apiCredentials.apiSecret
          )
          .execute()
      }
      t
    }
  }

  implicit def authRequestBuilder[T](r: FakeRequest[T]) = new AuthRequest(r)

  class AuthRequest[T](r: FakeRequest[T]) {

    def authorizedBy(user: CosmosProfile): FakeRequest[T] = authorizedBy(user.apiCredentials)

    def authorizedBy(credentials: ApiCredentials) = r.withHeaders(
      "Authorization" -> BasicAuth(credentials.apiKey, credentials.apiSecret))
  }
}
