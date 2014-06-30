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

import scala.concurrent.Future

import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonAST.{JValue, JString, JArray}
import net.liftweb.json.Extraction._

import es.tid.cosmos.servicemanager.ServiceError
import es.tid.cosmos.servicemanager.configuration.{HeaderOnlyConfiguration, Configuration}

/**
 * Wraps Ambari's cluster-related REST API calls.
 *
 * @constructor
 * @param clusterInfo   the Ambari JSON response that describes the cluster
 * @param serverBaseUrl the base url that describes the server
 */
private[ambari] class Cluster(clusterInfo: JValue, serverBaseUrl: Request)
  extends RequestProcessor {
  val name = clusterInfo \ "Clusters" \ "cluster_name" match {
    case JString(clusterName) => clusterName
    case other => throw new ServiceError("Ambari's cluster information response doesn't contain a " +
      s"Clusters/cluster_name element. Response: ${pretty(render(other))}")
  }

  private[this] def baseUrl: RequestBuilder = new RequestBuilder(serverBaseUrl) / "clusters" / name

  val serviceNames: Seq[String] = as.FlatValues(clusterInfo, "services", "service_name")

  val hostNames: Seq[String] = as.FlatValues(clusterInfo, "hosts", "host_name")

  /**
   * Configurations that have been added to the cluster (not necessary applied).
   */
  val configurations = for {
    JArray(configurations) <- clusterInfo \ "configurations"
    configuration <- configurations
    JString(tag) <- configuration \ "tag"
    JString(configType) <- configuration \ "type"
  } yield HeaderOnlyConfiguration(configType, tag)

  def getService(serviceName: String): Future[ServiceClient] =
    performRequest(baseUrl / "services" / serviceName).map(new ServiceClient(_, baseUrl.build))

  def addService(serviceName: String): Future[ServiceClient] =
    performRequest(baseUrl / "services" << s"""{"ServiceInfo": {"service_name": "$serviceName"}}""")
      .flatMap(_ => getService(serviceName))

  /**
   * Apply (which will also add) a configuration with the given type,
   * tag and properties to the cluster.
   */
  def applyConfiguration(configuration: Configuration, versionTag: String): Future[Unit] = {
    implicit val formats = net.liftweb.json.DefaultFormats
    val propertiesString = compact(render(decompose(configuration.properties)))
    val requestBody = s"""
      {
        "Clusters": {
          "desired_configs": {
            "type": "${configuration.configType}",
            "tag": "$versionTag",
            "properties": $propertiesString
          }
        }
      }"""
    performRequest(baseUrl.PUT.setBody(requestBody)).map(_ => ())
  }

  def getHost(hostName: String): Future[Host] =
    performRequest(baseUrl / "hosts" / hostName).map(new Host(_, baseUrl.build))

  def getHosts = Future.sequence(hostNames.map(getHost))

  def addHost(hostName: String): Future[Host] =
    performRequest(baseUrl / "hosts" << s"""{"Hosts":{"host_name":"$hostName"}}""")
      .flatMap(_ => getHost(hostName))

  def addHosts(hostNames: Seq[String]): Future[Seq[Host]] = Future.traverse(hostNames)(addHost)
}
