/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
