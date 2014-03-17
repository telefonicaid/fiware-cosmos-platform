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

import scala.concurrent.Future

import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST.{JString, JValue}
import net.liftweb.json.{compact, render}
import net.liftweb.json.JsonDSL._

import es.tid.cosmos.servicemanager.ServiceError

/** Wraps Ambari's service-related REST API calls.
  *
  * @constructor
  * @param serviceInfo    the Ambari JSON response that describes the service
  * @param clusterBaseUrl the base url that describes the cluster
  */
private[ambari] class ServiceClient (serviceInfo: JValue, clusterBaseUrl: Request)
  extends RequestProcessor with ServiceRequestHandlerFactory {
  val name = extractInfo("service_name")
  val state = extractInfo("state")

  override val toString = s"Service($name)[Cluster(${extractInfo("cluster_name")})]"

  private def baseUrl = new RequestBuilder(clusterBaseUrl) / "services" / name

  def addComponent(componentName: String): Future[String] =
    performRequest(baseUrl / "components" / componentName << "")
      .map(_ => componentName)

  def install(): Future[ServiceClient] = changeState("INSTALLED")

  def stop() = install()

  def start(): Future[ServiceClient] = changeState("STARTED")

  private def changeState(state: String) = {
    val body = compact(render("ServiceInfo" -> ("state" -> state)))
    performRequest(baseUrl.PUT.setBody(body))
      .flatMap(ensureFinished)
  }

  private def createAuthenticatedRequest(url: String) = baseUrl.setUrl(url)

  private def ensureFinished(json: JValue) = json \ "href" match {
    case JString(href) => {
      val request = createAuthenticatedRequest(href)
      createRequestHandler(request).ensureFinished.map(_ => this)
    }
    case _ => Future.successful(this)
  }

  private def extractInfo(key: String) = serviceInfo \ "ServiceInfo" \ key match {
    case JString(value) => value
    case _ => throw new ServiceError("Ambari's state information response doesn't contain a " +
      s"ServiceInfo/$key element")
  }
}
