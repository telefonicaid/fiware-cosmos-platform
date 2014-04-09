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
