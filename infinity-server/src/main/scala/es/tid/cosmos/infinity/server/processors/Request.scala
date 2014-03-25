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

package es.tid.cosmos.infinity.server.processors

import java.net.InetSocketAddress

import akka.actor.ActorRef
import spray.http.HttpRequest

import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.actions.Action

case class Request(
    remoteAddress: InetSocketAddress, responder: ActorRef, httpRequest: HttpRequest) {
  val action: Action = Action(remoteAddress, httpRequest)
  val credentials: Credentials = action.credentials
}
