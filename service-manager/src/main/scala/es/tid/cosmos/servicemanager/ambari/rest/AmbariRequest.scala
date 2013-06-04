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
    val FINISHED, WAITING, ERROR = Value

    /**
     * Combines the states of two operations and returns the state of the operation
     * that aggregates both.
     */
    def combine(left: Status.Value, right: Status.Value) = (left, right) match {
      case (Status.FINISHED, other) => other
      case (Status.ERROR, _) => Status.ERROR
      case (_, Status.ERROR) => Status.ERROR
      case (Status.WAITING, _) => Status.WAITING
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
        case Status.WAITING => future { blocking {
          Thread.sleep(1000)
          Await.result(ensureFinished, Duration.Inf)
        }}
        case Status.FINISHED => Future.successful()
        case Status.ERROR => Future.failed(DeploymentException(url.getUrl))
      })
  }
}
