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
import scala.concurrent._
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.{JString, JValue}
import scala.concurrent.duration.Duration

trait RequestHandlerFactory {
  def createRequestHandler(url: RequestBuilder): RequestHandler = new AmbariRequest(url)

  trait RequestHandler {
    def ensureFinished: Future[Unit]
  }

  class AmbariRequest(url: RequestBuilder) extends JsonHttpRequest with RequestHandler {
    private object Status extends Enumeration {
      type Status = Value
      val FINISHED, WAITING, ERROR = Value
      def fromString(str: String): Status.Value = str match {
        case "COMPLETED" => Status.FINISHED
        case "FAILED" | "TIMEDOUT" | "ABORTED" => Status.ERROR
        case "PENDING" | "QUEUED" | "IN_PROGRESS" => Status.WAITING
        case _ => throw new InternalError("Unexpected status string from Amabari: " + str)
      }

      def combine(prev: Status.Value, next: Status.Value): Status.Value = prev match {
        case Status.FINISHED => next
        case Status.ERROR => Status.ERROR
        case Status.WAITING => next match {
          case Status.ERROR => Status.ERROR
          case _ => Status.WAITING
        }
      }
    }

    private def extractStatusString(tasksObj: JValue): String  = (tasksObj \ "Tasks" \ "status") match {
      case JString(statusStr) => statusStr
      case _ => throw new InternalError("Ambari's request information response doesn't contain a Tasks/status element")
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
        } }
        case Status.FINISHED => Future.successful(())
        case Status.ERROR => Future.failed[Unit](new ServiceException(
          s"The cluster did not finish installing correctly. See ${url.build.getUrl} for more information"))
      })
    }
  }
}
