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

import java.net.InetAddress

import akka.actor.ActorRef
import spray.http.HttpRequest

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication._

case class Request(action: Action, credentials: Credentials, responder: ActorRef)

object Request {
  def apply(remoteAddress: InetAddress, httpRequest: HttpRequest, responder: ActorRef): Request =
    Request(Action(httpRequest), Credentials.from(remoteAddress, httpRequest), responder)
}
