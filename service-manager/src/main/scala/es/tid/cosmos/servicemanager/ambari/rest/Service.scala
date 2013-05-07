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

import es.tid.cosmos.servicemanager.Bug

/**
 * Wraps Ambari's service-related REST API calls.
 *
 * @constructor
 * @param serviceInfo    the Ambari JSON response that describes the service
 * @param clusterBaseUrl the base url that describes the cluster
 */
class Service private[ambari](serviceInfo: JValue, clusterBaseUrl: Request)
  extends JsonHttpRequest with RequestHandlerFactory {
  val name = serviceInfo \ "ServiceInfo" \ "service_name" match {
    case JString(serviceName) => serviceName
    case _ => throw new Bug("Ambari's state information response doesn't contain a " +
      "ServiceInfo/service_name element")
  }

  private def baseUrl = new RequestBuilder(clusterBaseUrl) / "services" / name

  def addComponent(componentName: String): Future[String] =
    performRequest(baseUrl / "components" / componentName << "")
      .map(_ => componentName)

  def install(): Future[Service] = changeState("INSTALLED")

  def stop() = install()

  def start(): Future[Service] = changeState("STARTED")

  private def changeState(state: String) = {
    val body = compact(render("ServiceInfo" -> ("state" -> state)))
    performRequest(baseUrl.PUT.setBody(body))
      .flatMap(ensureFinished)
  }

  private def ensureFinished(json: JValue) = {
    val requestUrl = baseUrl.setUrl(json \ "href" match {
      case JString(href) => href
      case _ => throw new Bug("Ambari's response doesn't contain a href element")
    })
    createRequestHandler(requestUrl).ensureFinished.map(_ => this)
   }
}
