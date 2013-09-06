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

package es.tid.cosmos.api.controllers.pages

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc.Session

import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.oauth2.UserProfile
import es.tid.cosmos.api.profile.UserId

class CosmosSessionTest extends FlatSpec with MustMatchers {
  val profile = UserProfile(id=UserId("db-1234"), name = Some("John Smith"))

  trait WithSession {
    def session: CosmosSession = new Session
  }

  trait WithUserId extends WithSession {
    override def session = super.session.setUserProfile(profile)
  }

  trait WithCosmosId extends WithSession {
    override def session = super.session.setCosmosId(42)
  }

  "An empty cosmos session" must "be not authenticated nor registered" in new WithSession {
    session must not be 'authenticated
    session must not be 'registered
  }

  it must "store and retrieve the Cosmos id" in new WithSession {
    session.setCosmosId(42L).cosmosId must be (Some(42L))
    session.setCosmosId(Some(42L)).cosmosId must be (Some(42L))
  }

  it must "store and retrieve the OAuth token" in new WithSession {
    session.setToken("token").token must be (Some("token"))
  }

  it must "store and retrieve the user profile" in new WithSession {
    session.setUserProfile(profile).userProfile must be (Some(profile))
  }

  "A cosmos session with a user id but no cosmos id" must "be authenticated but not registered" in
    new WithUserId {
      session must be ('authenticated)
      session must not be ('registered)
    }

  "A cosmos session with user and cosmos id" must "be authenticated and registered" in
    new WithUserId with WithCosmosId {
      session must be ('authenticated)
      session must be ('registered)
    }
}
