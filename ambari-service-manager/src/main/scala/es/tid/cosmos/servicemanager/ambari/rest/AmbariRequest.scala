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

import scala.concurrent._
import scala.concurrent.duration.Duration

import com.ning.http.client.{Request, RequestBuilder}
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.JValue

import es.tid.cosmos.servicemanager.DeploymentException

/**
 * Handles pending Ambari requests and lets you block until the request finishes.
 */
private[ambari] abstract class AmbariRequest(url: Request) extends RequestProcessor with RequestHandler {
  protected object Status extends Enumeration {
    type Status = Value
    val Finished, Waiting, Error = Value

    /**
     * Combines the states of two operations and returns the state of the operation
     * that aggregates both.
     */
    def combine(left: Status.Value, right: Status.Value) = (left, right) match {
      case (Status.Finished, other) => other
      case (Status.Error, _) => Status.Error
      case (_, Status.Error) => Status.Error
      case (Status.Waiting, _) => Status.Waiting
    }
  }

  protected def getStatusFromJson(statusJson: JValue): Status.Value

  protected def getRequest(url: Request): RequestBuilder

  /**
   * Returns a future that blocks until the request is finished. If the finished state is
   * not successful, the future contains a failed value.
   */
  override def ensureFinished: Future[Unit] = {
    performRequest(getRequest(url))
      .map(getStatusFromJson)
      .flatMap({
        case Status.Waiting => future { blocking {
          Thread.sleep(1000)
          Await.result(ensureFinished, Duration.Inf)
        }}
        case Status.Finished => Future.successful()
        case Status.Error => Future.failed(DeploymentException(url.getUrl))
      })
  }
}
