package es.tid.cosmos.infinity.server.authorization

import akka.actor.Props

import es.tid.cosmos.infinity.server.fs.InfinityDataStoreComponent

trait PersistentAuthorizationComponent extends AuthorizationComponent {
  this: InfinityDataStoreComponent =>

  override lazy val authorizationProps: Props =
    Props(new PersistentAuthorizationActor(new PersistentAuthorization(infinityDataStore)))
}
