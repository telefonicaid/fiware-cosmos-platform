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

import dispatch.{Future => _, _}
import com.ning.http.client.{Request, RequestBuilder}
import net.liftweb.json.JsonAST.{JString, JValue}

import es.tid.cosmos.servicemanager.ServiceError

private[ambari] class ServiceRequest(url: Request) extends AmbariRequest(url) {
  private def extractStatusString(tasksObj: JValue) = (tasksObj \ "Tasks" \ "status") match {
    case JString(statusStr) => statusStr
    case _ => throw new ServiceError(
      "Ambari's request information response doesn't contain a Tasks/status element")
  }

  private def statusFromString(str: String) = str match {
    case "COMPLETED" => Status.Finished
    case "FAILED" | "TIMEDOUT" | "ABORTED" => Status.Error
    case "PENDING" | "QUEUED" | "IN_PROGRESS" => Status.Waiting
    case _ => throw new ServiceError(s"Unexpected status string from Ambari: $str")
  }

  override protected def getStatusFromJson(statusJson: JValue) = (statusJson \ "tasks").children
    .map(extractStatusString)
    .map(statusFromString)
    .foldLeft(Status.Finished)(Status.combine)

  protected def getRequest(url: Request): RequestBuilder =
    new RequestBuilder(url) <<? Map("fields" -> "tasks/*")
}
