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

import net.liftweb.json.JsonAST._
import net.liftweb.json.{compact, render}
import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import scala.concurrent.Future
import es.tid.cosmos.servicemanager.InternalError

class Cluster(clusterInfo: JValue, serverBaseUrl: Request) extends JsonHttpRequest {
  val name = clusterInfo \ "Clusters" \ "cluster_name" match {
    case JString(clusterName) => clusterName
    case _ =>
      throw new InternalError("Ambari's cluster information response doesn't contain a Clusters/cluster_name element")
  }

  private[this] def baseUrl: RequestBuilder = new RequestBuilder(serverBaseUrl) / "clusters" / name

  val serviceNames = for {
    JString(serviceName) <- clusterInfo \\ "service_name"
  } yield serviceName

  val hostNames = for {
    JString(hostName) <- clusterInfo \\ "host_name"
  } yield hostName

  val configurations = for {
    JArray(configurations) <- clusterInfo \ "configurations"
    configuration <- configurations
    JString(tag) <- configuration \ "tag"
    JString(configType) <- configuration \ "type"
  } yield new Configuration(configType, tag)

  def getService(serviceName: String): Future[Service] = performRequest(baseUrl / "services" / serviceName)
    .map(new Service(_, baseUrl.build))

  def addService(serviceName: String): Future[Service] =
    performRequest(baseUrl / "services" << s"""{"ServiceInfo": {"service_name": "$serviceName"}}""")
      .flatMap(_ => getService(serviceName))

  def applyConfiguration(configType: String, tag: String, properties: JObject): Future[Unit] = {
    val propertiesString = compact(render(properties))
    val requestBody = s"""
      {
        "Clusters": {
          "desired_configs": {
            "type": "$configType",
            "tag": "$tag",
            "properties": $propertiesString
          }
        }
      }"""
    performRequest(baseUrl.PUT.setBody(requestBody)).map(_ => ())
  }

  def getHost(hostName: String) : scala.concurrent.Future[Host] =
    performRequest(baseUrl / "hosts" / hostName)
      .map(new Host(_, baseUrl.build))

  def addHost(hostName: String): scala.concurrent.Future[Host] =
    performRequest(baseUrl / "hosts" << s"""{"Hosts":{"host_name":"$hostName"}}""")
      .flatMap(_ => getHost(hostName))

}
