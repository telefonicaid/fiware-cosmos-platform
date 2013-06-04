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

import com.ning.http.client.RequestBuilder

private[ambari] trait RequestHandler {
  def ensureFinished: Future[Unit]
}

private[ambari] trait RequestHandlerFactory {
  def createRequestHandler(url: RequestBuilder): RequestHandler
}

private[ambari] trait ServiceRequestHandlerFactory extends RequestHandlerFactory {
  def createRequestHandler(url: RequestBuilder): RequestHandler = new ServiceRequest(url.build)
}

private[ambari] trait BootstrapRequestHandlerFactory extends RequestHandlerFactory {
  def createRequestHandler(url: RequestBuilder): RequestHandler = new BootstrapRequest(url.build)
}
