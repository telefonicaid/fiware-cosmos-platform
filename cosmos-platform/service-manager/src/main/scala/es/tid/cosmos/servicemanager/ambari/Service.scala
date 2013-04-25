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

import net.liftweb.json.JsonAST.{JString, JValue}
import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import scala.concurrent.{Future, future, blocking}

class Service(serviceInfo: JValue, clusterBaseUrl: Request) extends JsonHttpRequest {
  val name = serviceInfo \ "ServiceInfo" \ "service_name" match {
    case JString(serviceName) => serviceName
    case _ =>
      throw new InternalError("Ambari's state information response doesn't contain a ServiceInfo/service_name element")
  }
  private[this] def baseUrl = new RequestBuilder(clusterBaseUrl) / "services" / name

  def addComponent(componentName: String): Future[String] =
    performRequest(baseUrl / "components" / componentName << "").map(_ => componentName)

  def install(): Future[Service] =
    performRequest(baseUrl.PUT.setBody("""{"ServiceInfo": {"state": "INSTALLED"}}""")).flatMap(ensureFinished)

  def stop() = install

  def start(): Future[Service] =
    performRequest(baseUrl.PUT.setBody("""{"ServiceInfo": {"state": "STARTED"}}""")).flatMap(ensureFinished)

  private[this] def ensureFinished(json: JValue): Future[Service] = {
      val requestUrl = baseUrl.setUrl(json \ "href" match {
        case JString(href) => href
        case _ => throw new InternalError("Ambari's response doesn't contain a href element")
      })
      object Status extends Enumeration {
        type Status = Value
        val FINISHED, WAITING, ERROR = Value
        def fromString(str: String): Status.Value = str match {
          case "COMPLETED" => Status.FINISHED
          case "FAILED" | "TIMEDOUT" | "ABORTED" => Status.ERROR
          case "PENDING" | "QUEUED" | "IN_PROGRESS" => Status.WAITING
          case _ => throw new InternalError("Unexpected status string from Amabari: " + str)
        }
      }
      def getNewStatus(prev: Status.Value, next: Status.Value): Status.Value = prev match {
        case Status.FINISHED => next
        case Status.ERROR => Status.ERROR
        case Status.WAITING => next match {
          case Status.ERROR => Status.ERROR
          case _ => Status.WAITING
        }
      }
      def extractStatusString(tasksObj: JValue): String  = (tasksObj \ "Tasks" \ "status") match {
        case JString(statusStr) => statusStr
        case _ => throw new InternalError("Ambari's task information response doesn't contain a Tasks/status element")
      }
      def getStatusFromJson(statusJson: JValue): Status.Value = (statusJson \ "tasks").children
          .map(extractStatusString)
          .foldLeft(Status.FINISHED)((status, str) => getNewStatus(status, Status.fromString(str)))


      performRequest(requestUrl <<? Map("fields" -> "tasks/*"))
        .map(getStatusFromJson)
        .flatMap({
          case Status.WAITING => future { blocking {
            Thread.sleep(1000)
            ensureFinished(json)()
          } }
          case Status.FINISHED => Future.successful(this)
          case Status.ERROR => Future.failed[Service](new ServiceException(
            s"The cluster did not finish installing correctly. See ${requestUrl.build.getUrl} for more information"))
        })
    }
}
