package es.tid.cosmos.infinity.server.authentication.cosmosapi

import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent
import akka.actor.Props

trait CosmosApiAuthenticationComponent extends AuthenticationComponent {
  override lazy val authenticationProps: Props = ???
}
