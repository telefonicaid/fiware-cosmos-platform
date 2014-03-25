package es.tid.cosmos.infinity.server.authentication

import akka.actor.Props

trait AuthenticationComponent {
  def authenticationProps: Props
}
