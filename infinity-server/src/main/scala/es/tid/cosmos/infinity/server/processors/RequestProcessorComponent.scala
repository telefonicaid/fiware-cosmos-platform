package es.tid.cosmos.infinity.server.processors

import akka.actor.{Props, ActorRef}

trait RequestProcessorComponent {
  def requestProcessorProps(authenticationRef: ActorRef, authorizationRef: ActorRef): Props
}
