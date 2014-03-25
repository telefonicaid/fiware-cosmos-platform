package es.tid.cosmos.infinity.server.authorization

import akka.actor.Actor

/** Dummy implementation to be replaced by full-fledged authorization */
private[authorization] class PersistentAuthorization extends Actor {

  override def receive: Receive = {
    case _ =>
  }
}
