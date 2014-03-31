package es.tid.cosmos.infinity.server.processors

import akka.actor.{Props, ActorRef}

trait RequestProcessorComponent {
  def requestProcessorProps(authenticationProps: Props, authorizationProps: Props): Props
}
