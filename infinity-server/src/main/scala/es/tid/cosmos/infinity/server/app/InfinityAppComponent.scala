/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

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
