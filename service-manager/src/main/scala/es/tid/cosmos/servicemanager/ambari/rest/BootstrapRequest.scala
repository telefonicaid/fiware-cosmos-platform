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

import com.ning.http.client.{Request, RequestBuilder}
import net.liftweb.json.JsonAST.{JString, JValue}

import es.tid.cosmos.servicemanager.ServiceError

class BootstrapRequest(url: Request) extends AmbariRequest(url) {
  private def statusFromString(str: String) = str match {
    case "RUNNING" => Status.WAITING
    case "SUCCESS" => Status.FINISHED
    case "ERROR" => Status.ERROR
    case _ => throw ServiceError(s"Unexpected request status in bootstrap request: $str")
  }

  private def extractStatusString(statusObj: JValue) = statusObj match {
    case JString(statusStr) => statusStr
    case _ => throw new ServiceError(
      "Ambari's request information response doesn't contain a status element")
  }

  override protected def getStatusFromJson(statusJson: JValue) =
    statusFromString(extractStatusString(statusJson \ "status"))

  protected def getRequest(url: Request): RequestBuilder = new RequestBuilder(url)
}
