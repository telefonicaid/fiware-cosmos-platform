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

import dispatch.{Future => _, _}, Defaults._
import scala.concurrent.Future
import net.liftweb.json.JsonAST._
import com.ning.http.client.RequestBuilder

class AmbariServer(serverUrl: String, port: Int, username: String, password: String) extends ProvisioningServer with JsonHttpRequest {

  private[this] def baseUrl: RequestBuilder = host(serverUrl, port).as_!(username, password) / "api" / "v1"

  override def listClusterNames: Future[Seq[String]] = performRequest(baseUrl / "clusters").map(json => for {
    JField("cluster_name", JString(name)) <- (json \\ "cluster_name").children
  } yield name)

  override def getCluster(name: String): Future[Cluster] = performRequest(baseUrl / "clusters" / name)
    .map({
    new Cluster(_, baseUrl.build)
  })

  override def createCluster(name: String, version: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name << s"""{"Clusters": {"version": "$version"}}""")
      .flatMap(_ => getCluster(name))

  override def removeCluster(name: String): Future[JValue] =
    performRequest(baseUrl.DELETE / "clusters" / name)
}
