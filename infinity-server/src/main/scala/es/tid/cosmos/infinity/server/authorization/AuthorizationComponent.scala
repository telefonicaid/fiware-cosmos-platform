package es.tid.cosmos.infinity.server.authorization

import akka.actor.Props

trait AuthorizationComponent {
  def authorizationProps: Props
}
