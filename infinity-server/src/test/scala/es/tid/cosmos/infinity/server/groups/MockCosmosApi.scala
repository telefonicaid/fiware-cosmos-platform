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

package es.tid.cosmos.infinity.server.groups

import scala.concurrent.duration._

import org.scalatest.Assertions
import unfiltered.filter.Planify
import unfiltered.jetty.Http
import unfiltered.request._
import unfiltered.response._

import es.tid.cosmos.common.BearerToken

class MockCosmosApi(port: Int) extends Assertions {

  private var infinitySecret: String = "default"
  private var groupMapping: Map[String, String] = Map.empty
  private var redirectTo: Option[String] = None
  private var processingDelay: FiniteDuration = 0.seconds

  def givenInfinitySecret(secret: String): Unit = {
    infinitySecret = secret
  }

  def givenUserGroups(handle: String, groups: Seq[String]): Unit = {
    val body = groups.mkString("{ \"groups\": [\"", "\", \"", "\"]}")
    groupMapping += handle -> body
  }

  def givenResponseWillBeMalformed(handle: String): Unit = {
    groupMapping += handle -> "]malformed {response["
  }

  def givenRedirectionResponse(target: String): Unit = {
    redirectTo = Some(target)
  }

  def givenResponseIsDelayed(delay: FiniteDuration): Unit = {
    processingDelay = delay
  }

  def withServer[T](block: => T): T = {
    val server = defineServer()
    try {
      server.start()
      block
    } finally {
      server.stop().join()
    }
  }

  private def defineServer() = {
    object Handle extends Params.Extract("handle", Params.first)

    Http.local(port).filter(Planify {
      case req @ Authorization(auth) & GET(Path("/infinity/v1/groups") & Params(Handle(handle))) =>
        auth match {
          case BearerToken(token) if token == infinitySecret =>
            if (redirectTo.isDefined) Redirect(redirectTo.get)
            else {
              Thread.sleep(processingDelay.toMillis)
              groupMapping.get(handle) match {
                case None => BadRequest ~> ResponseString("Handle not found")
                case Some(response) => Ok ~> ResponseString(response)
              }
            }
          case _ => Forbidden
        }

      case req => fail(s"Unexpected request $req")
    })
  }
}
