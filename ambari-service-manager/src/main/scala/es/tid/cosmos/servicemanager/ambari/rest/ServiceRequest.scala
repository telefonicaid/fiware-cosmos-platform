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
