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
import net.liftweb.json._
import net.liftweb.json.JsonAST.JString
import net.liftweb.json.JsonDSL._

import es.tid.cosmos.servicemanager.ServiceError

/**
 * Wraps Ambari's host-related REST API calls.
 *
 * @param hostInfo       the Ambari JSON response that describes the host
 * @param clusterBaseUrl the base url of the cluster
 */
private[ambari] class Host private[ambari](hostInfo: JValue, clusterBaseUrl: Request) extends RequestProcessor {
  val name = hostInfo \ "Hosts" \ "public_host_name" match {
    case JString(hostName) => hostName
    case _ =>
      throw new ServiceError("Ambari's host information response doesn't contain a " +
        "Hosts/public_host_name element")
  }

  /**
   * Given a sequence of component names, add each component to the host. The services of each of
   * the components must have been added to the cluster previously.
   */
  def addComponents(componentNames: String*): Future[Unit] = {
    def getJsonForComponent(componentName: String) =
      ("HostRoles" -> ("component_name" -> componentName))
    def ignoreResult(result: JValue) {}
    if (!componentNames.isEmpty)
      performRequest(new RequestBuilder(clusterBaseUrl) / "hosts"
          <<? Map("Hosts/host_name" -> name)
          << compact(render(("host_components" -> componentNames.map(getJsonForComponent)))))
        .map(ignoreResult)
    else Future.successful()
  }

  def getComponentNames: Seq[String] = as.FlatValues(hostInfo,  "host_components", "component_name")
}
