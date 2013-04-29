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

import com.ning.http.client.RequestBuilder
import net.liftweb.json.JsonAST.JValue
import scala.concurrent._
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.JString
import scala.concurrent.duration.Duration
import es.tid.cosmos.servicemanager.Bug

class AmbariRequest(url: RequestBuilder) extends JsonHttpRequest with RequestHandler {
  private object Status extends Enumeration {
    type Status = Value
    val FINISHED, WAITING, ERROR = Value
    def fromString(str: String): Status.Value = str match {
      case "COMPLETED" => Status.FINISHED
      case "FAILED" | "TIMEDOUT" | "ABORTED" => Status.ERROR
      case "PENDING" | "QUEUED" | "IN_PROGRESS" => Status.WAITING
      case _ => throw new Bug(s"Unexpected status string from Ambari: $str")
    }

    /**
     * Combines the states of two operations and returns the state of the operation
     * that aggregates both.
     */
    def combine(left: Status.Value, right: Status.Value): Status.Value = (left, right) match {
      case (Status.FINISHED, other) => other
      case (Status.ERROR, _) => Status.ERROR
      case (_, Status.ERROR) => Status.ERROR
      case (Status.WAITING, _) => Status.WAITING
    }
  }

  private def extractStatusString(tasksObj: JValue): String  = (tasksObj \ "Tasks" \ "status") match {
    case JString(statusStr) => statusStr
    case _ => throw new Bug("Ambari's request information response doesn't contain a Tasks/status element")
  }

  private def getStatusFromJson(statusJson: JValue): Status.Value = (statusJson \ "tasks").children
    .map(extractStatusString)
    .foldLeft(Status.FINISHED)((status, str) => Status.combine(status, Status.fromString(str)))

  override def ensureFinished: Future[Unit] = {
    performRequest(url <<? Map("fields" -> "tasks/*"))
      .map(getStatusFromJson)
      .flatMap({
      case Status.WAITING => future { blocking {
        Thread.sleep(1000)
        Await.result(ensureFinished, Duration.Inf)
      }}
      case Status.FINISHED => Future.successful(())
      case Status.ERROR => Future.failed[Unit](new ServiceException(
        s"The cluster did not finish installing correctly. See ${url.build.getUrl} for more information"))
    })
  }
}
