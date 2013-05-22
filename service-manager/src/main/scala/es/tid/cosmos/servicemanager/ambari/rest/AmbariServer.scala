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

import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.{compact, render}
import net.liftweb.json.JsonAST._
import net.liftweb.json.JsonDSL._
import com.ning.http.client.RequestBuilder

import es.tid.cosmos.platform.ial.MachineState
import es.tid.cosmos.servicemanager.{ServiceError, RequestException}

/**
 * Root class that enables REST calls to the Ambari server.
 *
 * @constructor
 * @param serverUrl the url of the server
 * @param port      the port where the server is listening to REST calls
 * @param username  the username used for authentication
 * @param password  the password used for authentication
 */
class AmbariServer(serverUrl: String, port: Int, username: String, password: String)
  extends ClusterProvisioner with RequestProcessor with BootstrapRequestHandlerFactory {

  private[this] def baseUrl = host(serverUrl, port).as_!(username, password) / "api" / "v1"

  override def listClusterNames: Future[Seq[String]] =
    performRequest(baseUrl / "clusters").map(json => for {
      JField("cluster_name", JString(name)) <- (json \\ "cluster_name").children
    } yield name)

  override def getCluster(name: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name).map(new Cluster(_, baseUrl.build))

  override def createCluster(name: String, version: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name << s"""{"Clusters": {"version": "$version"}}""")
      .flatMap(_ => getCluster(name))

  override def removeCluster(name: String): Future[Unit] =
    performRequest(baseUrl.DELETE / "clusters" / name).map(_ => ())

  private def performBootstrapAction(
    machines: Seq[MachineState],
    sshKey: String,
    builderWithMethod: RequestBuilder): Future[Unit] = {
    val hosts = compact(render(machines.map(_.hostname)))
    val configuredBuilder = (builderWithMethod / "bootstrap")
      .setBody(s"""{"hosts": $hosts, "sshKey": "$sshKey", "verbose": true}""")
      .addHeader("Content-Type", "application/json")
    performRequest(configuredBuilder)
      .map(response => response \ "status" match {
      case JString("OK") => {
        val JInt(a) = (response \ "requestId")
        a.toInt
      }
      case JString("ERROR") => throw RequestException(
        configuredBuilder.build,
        s"Bootstrap request returned an error: ${response \\ "log"}")
      case _ => throw ServiceError(s"Unexpected Ambari response for bootstrap request: $response")
    }).flatMap(requestId => createRequestHandler(baseUrl / "bootstrap" / requestId).ensureFinished)
  }

  override def bootstrapMachines(machines: Seq[MachineState], sshKey: String): Future[Unit] =
    performBootstrapAction(machines, sshKey, baseUrl.POST)

  override def teardownMachines(machines: Seq[MachineState], sshKey: String): Future[Unit] =
    performBootstrapAction(machines, sshKey, baseUrl.DELETE)

  override def registeredHostnames: Future[Seq[String]] =
    performRequest(baseUrl / "hosts").map(json =>
      (json \ "items").children.map(item =>
        item \\ "host_name" match {
          case JString(hostname) => hostname
          case _ => throw ServiceError("Ambari's host response does not contain a host_name element")
    }))
}
