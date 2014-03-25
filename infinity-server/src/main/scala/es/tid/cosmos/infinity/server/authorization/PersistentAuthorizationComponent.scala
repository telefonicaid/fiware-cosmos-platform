package es.tid.cosmos.infinity.server.authorization

import akka.actor.Props

trait PersistentAuthorizationComponent extends AuthorizationComponent {
  override lazy val authorizationProps: Props = Props(new PersistentAuthorization)
}
