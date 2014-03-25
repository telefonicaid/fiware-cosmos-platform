package es.tid.cosmos.infinity.server.processors

import akka.actor.{Props, ActorRef}
import es.tid.cosmos.common.ConfigComponent

trait DefaultRequestProcessorComponent extends RequestProcessorComponent { this: ConfigComponent =>
  override def requestProcessorProps(authenticationRef: ActorRef, authorizationRef: ActorRef): Props =
    RequestProcessor.props(authenticationRef, authorizationRef, config)
}
