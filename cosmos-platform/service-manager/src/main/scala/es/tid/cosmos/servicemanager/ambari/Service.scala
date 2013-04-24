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
import es.tid.cosmos.servicemanager.InternalError

class Service(serviceInfo: JValue, clusterBaseUrl: Request) extends JsonHttpRequest {
  val name = serviceInfo \ "ServiceInfo" \ "service_name" match {
    case JString(serviceName) => serviceName
    case _ => throw new InternalError("Ambari's state information response doesn't contain a " +
      "ServiceInfo/service_name element")
  }

  private[this] def baseUrl = new RequestBuilder(clusterBaseUrl) / "services" / name

  def addComponent(componentName: String): Future[String] =
    performRequest(baseUrl / "components" / componentName << "").map(_ => componentName)

  def install: Future[Service] =
    performRequest(baseUrl.PUT.setBody("""{"ServiceInfo": {"state": "INSTALLED"}}"""))
      .flatMap(ensureFinished)

  def stop = install

  def start: Future[Service] =
    performRequest(baseUrl.PUT.setBody("""{"ServiceInfo": {"state": "STARTED"}}"""))
      .flatMap(ensureFinished)

  private[this] def ensureFinished(json: JValue): Future[Service] = {
    val requestUrl = baseUrl.setUrl(json \ "href" match {
      case JString(href) => href
      case _ => throw new InternalError("Ambari's response doesn't contain a href element")
    })
    new AmbariRequest(requestUrl).ensureFinished().map(_ => this)
   }
}
