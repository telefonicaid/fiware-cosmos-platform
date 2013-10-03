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
import es.tid.cosmos.api.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

class CosmosSessionTest extends FlatSpec with MustMatchers {
  val profile = OAuthUserProfile(id=UserId("db-1234"), name = Some("John Smith"))

  trait WithEmptySession {
    def session: CosmosSession = new Session
  }

  trait WithUserSession {
    def session: CosmosSession = Session().setUserProfile(profile)
  }

  "An empty cosmos session" must "be not authenticated" in new WithEmptySession {
    session must not be 'authenticated
  }

  it must "store and retrieve the OAuth token" in new WithEmptySession {
    session.setToken("token").token must be (Some("token"))
  }

  it must "store and retrieve the user profile" in new WithEmptySession {
    session.setUserProfile(profile).userProfile must be (Some(profile))
  }

  "A cosmos session with a user id but no cosmos id" must "be authenticated" in
    new WithUserSession {
      session must be ('authenticated)
    }
}
