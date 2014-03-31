package es.tid.cosmos.infinity.server.app

import akka.actor.Props

import es.tid.cosmos.infinity.server.authorization.AuthorizationComponent
import es.tid.cosmos.infinity.server.processors.RequestProcessorComponent
import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent

trait InfinityAppComponent {
  this: AuthenticationComponent with AuthorizationComponent with RequestProcessorComponent =>

  lazy val infinityAppProps: Props =
    Props(new InfinityApp(requestProcessorProps(authenticationProps, authorizationProps)))
}
