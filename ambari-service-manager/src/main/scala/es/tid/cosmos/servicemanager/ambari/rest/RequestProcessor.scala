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

package es.tid.cosmos.servicemanager.ambari.rest

import java.util.concurrent.ExecutionException
import scala.concurrent.{future, Future, blocking}
import scala.concurrent.duration._

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.{JObject, JValue}

import es.tid.cosmos.servicemanager.RequestException

private[ambari] trait RequestProcessor {

  private val MaxRetries = 3

  /** Executes the given request, handles error cases and returns the body as JSON in the success
    * case.
    */
  private def performRequest(request: RequestBuilder, retry: Int): Future[JValue] = {
    def handleFailure(throwable: Throwable) = throwable match {
      case ex: ExecutionException if ex.getCause.isInstanceOf[StatusCode] =>
        RequestException(
          request.build,
          s"""Error when performing Http request.
          |Http code ${ex.getCause.asInstanceOf[StatusCode].code}
          |Body: ${request.build.getStringData}"
          |Message: ${ex.getMessage}""".stripMargin,
          ex.getCause)
      case other => other
    }

    /** This function takes care of the cases where Ambari returns an empty JSON due to
      * timing issues.
      */
    def handleAmbariTimingBugs(response: JValue) = {
      if (response == JObject(List.empty) && retry <= MaxRetries && request.build.getMethod == "GET") {
        for {
          _ <- future { blocking { Thread.sleep((500 milliseconds).toMillis * retry) } }
          result <- performRequest(request, retry + 1)
        } yield result
      } else {
        Future.successful(response)
      }
    }
    Http(request.OK(as.Json)).flatMap(handleAmbariTimingBugs).transform(identity, handleFailure)
  }

  def performRequest(request: RequestBuilder): Future[JValue] = performRequest(request, 1)
}
