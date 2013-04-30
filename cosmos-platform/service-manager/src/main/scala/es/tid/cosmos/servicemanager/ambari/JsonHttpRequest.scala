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

package es.tid.cosmos.servicemanager.ambari

import java.util.concurrent.ExecutionException

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}, Defaults._

trait JsonHttpRequest {
  def performRequest(request: RequestBuilder) = {
    def handleFailure(throwable: Throwable) = throwable match {
      case ex: ExecutionException if ex.getCause.isInstanceOf[StatusCode] => new ServiceException(
        s"""Error when performing Http request. Http code ${ex.getCause.asInstanceOf[StatusCode].code}
          AmbariRequest: ${request.build.toString}
          Body: ${request.build.getStringData}
          """, ex.getCause)
      case other => other
    }
    Http(request.OK(asJson)).transform(identity, handleFailure)
  }
}
