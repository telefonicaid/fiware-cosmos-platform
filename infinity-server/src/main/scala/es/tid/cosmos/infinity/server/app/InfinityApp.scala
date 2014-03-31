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

import akka.actor.{ActorRef, Props, ActorLogging, Actor}
import spray.http.MediaTypes._
import spray.http.HttpRequest
import spray.http.HttpHeaders.`Remote-Address`
import spray.routing._

import es.tid.cosmos.infinity.server.processors.Request

class InfinityApp(requestProcessorProps: Props)
  extends Actor with HttpService with ActorLogging {

  def actorRefFactory = context

  def receive = runRoute(routes)

  private val routes =
    pathPrefix("webhdfs" / "v1") {
      delegateToRequestProcessor
    } ~
    path("") {
      get {
        respondWithMediaType(`text/html`) {
          complete {
            <html>
              <head><h1>Infinity server</h1></head>
              <body>
                <h1>Infinity server</h1>
                <p>Running!</p>
              </body>
            </html>
          }
        }
      }
    }

  private def delegateToRequestProcessor(ctx: RequestContext): Unit =
    startRequestProcessor() ! parseRequest(ctx)

  private def parseRequest(ctx: RequestContext) = Request(
    remoteAddress = remoteAddress(ctx.request),
    responder = ctx.responder,
    httpRequest = ctx.request
  )

  private def remoteAddress(request: HttpRequest) = (for {
    header <- request.header[`Remote-Address`]
    address <- header.address.toOption
  } yield address).getOrElse(throw new IllegalArgumentException(
    "Remote address not found. This should not happen if " +
      "spray.can.server.remote-address-header setting is on"
  ))

  private def startRequestProcessor(): ActorRef =
    context.actorOf(requestProcessorProps)
}
