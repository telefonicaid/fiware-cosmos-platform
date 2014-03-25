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
import spray.routing._

class InfinityApp(
    authenticationProps: Props,
    authorizationProps: Props,
    requestProcessorProps: (ActorRef, ActorRef) => Props)
  extends Actor with HttpService with ActorLogging {

  private val authenticationRef = context.actorOf(authenticationProps)
  private val authorizationRef = context.actorOf(authorizationProps)

  def actorRefFactory = context

  def receive = runRoute(routes)

  private val routes =
    path("webhdfs/v1") {
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
    startRequestProcessor() ! ctx

  private def startRequestProcessor(): ActorRef =
    context.actorOf(requestProcessorProps(authenticationRef, authorizationRef))
}
