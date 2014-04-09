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

import akka.actor.Actor

import es.tid.cosmos.infinity.server.authorization.Authorization.Authorize

private[authorization] class PersistentAuthorizationActor(
    authorization: PersistentAuthorization) extends Actor {

  override def receive: Receive = {
    case Authorize(action, profile) =>
      sender ! authorization.authorize(action, profile)
      context.stop(self)
  }
}
