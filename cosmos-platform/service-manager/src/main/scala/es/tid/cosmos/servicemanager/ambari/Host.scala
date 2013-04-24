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

import net.liftweb.json.JsonAST.JString
import net.liftweb.json._
import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import scala.concurrent.Future
import es.tid.cosmos.servicemanager.InternalError

class Host(hostInfo: JValue, clusterBaseUrl: Request) extends JsonHttpRequest {
  val name = hostInfo \ "Hosts" \ "public_host_name" match {
    case JString(hostName) => hostName
    case _ =>
      throw new InternalError("Ambari's host information response doesn't contain a Hosts/public_host_name element")
  }

  private[this] def baseUrl = new RequestBuilder(clusterBaseUrl) / "hosts" / name

  def addComponents(componentNames: String*): Future[Unit] = {
    def getJsonForComponent(componentName: String): String = s"""{"HostRoles": {"component_name": "$componentName"}}"""
    val hostsJsonStr = componentNames
      .fold("[")((str, name) => str + getJsonForComponent(name) + ",")
      .dropRight(1) + "]"
    performRequest(new RequestBuilder(clusterBaseUrl) / "hosts"
      <<? Map("Hosts/host_name" -> name)
      << s"""{"host_components": $hostsJsonStr}""").map(_ => ())
  }
}
