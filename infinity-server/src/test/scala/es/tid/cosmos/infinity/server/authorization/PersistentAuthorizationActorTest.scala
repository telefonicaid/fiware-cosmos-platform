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

package es.tid.cosmos.infinity.server.authorization

import akka.actor.Props
import akka.testkit.ImplicitSender
import org.mockito.BDDMockito.given
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.{UserCredentials, UserProfile}
import es.tid.cosmos.infinity.server.authorization.AuthorizationProvider._
import es.tid.cosmos.infinity.server.util.Path
import es.tid.cosmos.infinity.test.ActorFlatSpec

class PersistentAuthorizationActorTest
  extends ActorFlatSpec("PersistentAuthorizationTest") with ImplicitSender with MockitoSugar {

  trait Fixture {
    val action = Action.Open(Path.absolute("/some/readable/path"), UserCredentials("key", "secret"))
    val userProfile = UserProfile("jsmith", "group", UnixFilePermissions.fromOctal("777"))
    val authorization = mock[PersistentAuthorization]
    val instance = system.actorOf(Props(new PersistentAuthorizationActor(authorization)))
    watch(instance)
  }

  "The authorization actor" must "authorize valid request and stop" in new Fixture {
    given(authorization.authorize(action, userProfile)).willReturn(Authorized)
    instance ! Authorize(action, userProfile)
    expectMsg(Authorized)
    expectTerminated(instance)
  }

  it must "reject invalid request and stop" in new Fixture {
    val authorizationFailed = AuthorizationFailed(new Error("Invalid"))
    given(authorization.authorize(action, userProfile)).willReturn(authorizationFailed)
    instance ! Authorize(action, userProfile)
    expectMsg(authorizationFailed)
    expectTerminated(instance)
  }
}
