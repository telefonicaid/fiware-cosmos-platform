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

package es.tid.cosmos.infinity.server

import akka.actor.{ActorLogging, Actor}
import spray.http.MediaTypes._
import spray.routing._
import akka.event.LoggingAdapter

class InfinityActor extends Actor with InfinityService with ActorLogging {

  def actorRefFactory = context

  def receive = runRoute(myRoute)
}

trait InfinityService extends HttpService {

  def log: LoggingAdapter

  def myRoute =
    path("") {
      get {
        log.info("Got a GET request")
        respondWithMediaType(`text/html`) { // XML is marshalled to `text/xml` by default, so we simply override here
          complete {
            <html>
              <body>
                <h1>Say hello to <i>spray-routing</i> on <i>spray-can</i>!</h1>
              </body>
            </html>
          }
        }
      }
    }
}
