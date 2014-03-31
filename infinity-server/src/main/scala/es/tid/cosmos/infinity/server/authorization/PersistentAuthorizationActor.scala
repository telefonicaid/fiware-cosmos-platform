package es.tid.cosmos.infinity.server.authorization

import akka.actor.Actor

import es.tid.cosmos.infinity.server.authorization.AuthorizationProvider.Authorize

private[authorization] class PersistentAuthorizationActor(
    authorization: PersistentAuthorization) extends Actor {

  override def receive: Receive = {
    case Authorize(action, profile) =>
      sender ! authorization.authorize(action, profile)
      context.stop(self)
  }
}
