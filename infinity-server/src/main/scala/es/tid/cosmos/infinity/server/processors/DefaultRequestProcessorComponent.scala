package es.tid.cosmos.infinity.server.processors

import akka.actor.{Props, ActorRef}
import es.tid.cosmos.common.ConfigComponent

trait DefaultRequestProcessorComponent extends RequestProcessorComponent { this: ConfigComponent =>
  override def requestProcessorProps(authenticationProps: Props, authorizationProps: Props): Props =
    RequestProcessor.props(authenticationProps, authorizationProps, config)
}
