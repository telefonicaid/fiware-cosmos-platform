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

import scala.concurrent.Future

import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonAST.{JValue, JString, JArray}
import net.liftweb.json.Extraction._

import es.tid.cosmos.servicemanager.{Bug, Configuration, HeaderOnlyConfiguration}


/**
 * Wraps Ambari's cluster-related REST API calls.
 *
 * @constructor
 * @param clusterInfo   the Ambari JSON response that describes the cluster
 * @param serverBaseUrl the base url that describes the server
 */
class Cluster private[ambari] (clusterInfo: JValue, serverBaseUrl: Request) extends JsonHttpRequest {
  val name = clusterInfo \ "Clusters" \ "cluster_name" match {
    case JString(clusterName) => clusterName
    case _ => throw new Bug("Ambari's cluster information response doesn't contain a " +
      "Clusters/cluster_name element")
  }

  private[this] def baseUrl: RequestBuilder = new RequestBuilder(serverBaseUrl) / "clusters" / name

  val serviceNames = for {
    JString(serviceName) <- clusterInfo \\ "service_name"
  } yield serviceName

  val hostNames = for {
    JString(hostName) <- clusterInfo \\ "host_name"
  } yield hostName

  /**
   * Configurations that have been added to the cluster (not necessary applied).
   */
  val configurations = for {
    JArray(configurations) <- clusterInfo \ "configurations"
    configuration <- configurations
    JString(tag) <- configuration \ "tag"
    JString(configType) <- configuration \ "type"
  } yield HeaderOnlyConfiguration(configType, tag)

  def getService(serviceName: String): Future[Service] =
    performRequest(baseUrl / "services" / serviceName).map(new Service(_, baseUrl.build))

  def addService(serviceName: String): Future[Service] =
    performRequest(baseUrl / "services" << s"""{"ServiceInfo": {"service_name": "$serviceName"}}""")
      .flatMap(_ => getService(serviceName))

  /**
   * Apply (which will also add) a configuration with the given type, tag and properties to the cluster.
   */
  def applyConfiguration(configuration: Configuration): Future[Unit] = {
    implicit val formats = net.liftweb.json.DefaultFormats
    val propertiesString = compact(render(decompose(configuration.properties)))
    val requestBody = s"""
      {
        "Clusters": {
          "desired_configs": {
            "type": "${configuration.configType}",
            "tag": "${configuration.tag}",
            "properties": $propertiesString
          }
        }
      }"""
    performRequest(baseUrl.PUT.setBody(requestBody)).map(_ => ())
  }

  def getHost(hostName: String): Future[Host] =
    performRequest(baseUrl / "hosts" / hostName).map(new Host(_, baseUrl.build))

  def addHost(hostName: String): Future[Host] =
    performRequest(baseUrl / "hosts" << s"""{"Hosts":{"host_name":"$hostName"}}""")
      .flatMap(_ => getHost(hostName))
}
