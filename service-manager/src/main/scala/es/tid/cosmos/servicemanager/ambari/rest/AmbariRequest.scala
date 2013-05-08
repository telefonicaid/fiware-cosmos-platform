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

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.{JValue, JString}

import es.tid.cosmos.servicemanager.{DeploymentException, ServiceError}

/**
 * Handles pending Ambari requests and lets you block until the request finishes.
 */
class AmbariRequest(url: RequestBuilder) extends RequestProcessor with RequestHandler {
  private object Status extends Enumeration {
    type Status = Value
    val FINISHED, WAITING, ERROR = Value
    def fromString(str: String) = str match {
      case "COMPLETED" => Status.FINISHED
      case "FAILED" | "TIMEDOUT" | "ABORTED" => Status.ERROR
      case "PENDING" | "QUEUED" | "IN_PROGRESS" => Status.WAITING
      case _ => throw new ServiceError(s"Unexpected status string from Ambari: $str")
    }

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

  private def extractStatusString(tasksObj: JValue)  = (tasksObj \ "Tasks" \ "status") match {
    case JString(statusStr) => statusStr
    case _ => throw new ServiceError(
      "Ambari's request information response doesn't contain a Tasks/status element")
  }

  private def getStatusFromJson(statusJson: JValue) = (statusJson \ "tasks").children
    .map(extractStatusString)
    .map(Status.fromString)
    .foldLeft(Status.FINISHED)(Status.combine)

  /**
   * Returns a future that blocks until the request is finished. If the finished state it not successful, the future
   * contains a failed value.
   */
  override def ensureFinished: Future[Unit] = {
    val request = url <<? Map("fields" -> "tasks/*")
    performRequest(request)
      .map(getStatusFromJson)
      .flatMap({
        case Status.WAITING => future { blocking {
          Thread.sleep(1000)
          Await.result(ensureFinished, Duration.Inf)
        }}
        case Status.FINISHED => Future.successful()
        case Status.ERROR => Future.failed(DeploymentException(url.build.getUrl))
      })
  }
}
