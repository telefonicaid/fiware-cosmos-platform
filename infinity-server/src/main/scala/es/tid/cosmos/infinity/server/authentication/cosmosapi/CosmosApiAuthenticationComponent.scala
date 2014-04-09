package es.tid.cosmos.infinity.server.authentication.cosmosapi

import akka.actor.{Actor, Props}

import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent

trait CosmosApiAuthenticationComponent extends AuthenticationComponent {
  override lazy val authenticationProps: Props = Props(new Actor {
    override def receive: Receive = {
      case _ =>
    }
  })
}
