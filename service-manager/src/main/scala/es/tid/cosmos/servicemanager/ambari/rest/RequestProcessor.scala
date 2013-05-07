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

package es.tid.cosmos.servicemanager.ambari.rest

import java.util.concurrent.ExecutionException
import scala.concurrent.Future

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.JValue
import es.tid.cosmos.servicemanager.RequestException

trait RequestProcessor {
  /**
   * Executes the given request, handles error cases and returns the body as JSON in the success case.
   */
  def performRequest(request: RequestBuilder): Future[JValue] = {
    def handleFailure(throwable: Throwable) = throwable match {
      case ex: ExecutionException if ex.getCause.isInstanceOf[StatusCode] => RequestException(
        request.build,
        s"""Error when performing Http request. Http code ${ex.getCause.asInstanceOf[StatusCode].code}
          Body: ${request.build.getStringData}""",
        ex.getCause)
      case other => other
    }
    Http(request.OK(as.Json)).transform(identity, handleFailure)
  }
}
