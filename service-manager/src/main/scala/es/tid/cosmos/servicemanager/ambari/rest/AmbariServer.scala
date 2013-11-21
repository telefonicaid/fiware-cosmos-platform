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

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import net.liftweb.json.JsonAST.{JField, JString}
import net.liftweb.json.render

import es.tid.cosmos.platform.common.SequentialOperations
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
private[ambari] class AmbariServer(serverUrl: String, port: Int, username: String, password: String)
  extends RequestProcessor with BootstrapRequestHandlerFactory {
  implicit private val formats = DefaultFormats

  private[this] def baseUrl = host(serverUrl, port).as_!(username, password) / "api" / "v1"

  /**
   * Get a list of the names of the existing, managed clusters.
   *
   * @return the future of the list of names
   */
  def listClusterNames: Future[Seq[String]] =
    performRequest(baseUrl / "clusters").map(json => as.FlatValues(json, "items", "cluster_name"))

  /**
   * Get the cluster specified by the given name.
   *
   * @param id the id of the cluster
   * @return the future of the cluster iff found
   */
  def getCluster(id: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / id).map(new Cluster(_, baseUrl.build))

  /**
   * Create a cluster.
   *
   * @param name the cluster's name
   * @param version the version of the Ambari Service stack, e.g. `"HDP-1.2.0"`
   * @return the future of the created cluster
   */
  def createCluster(name: String, version: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name << s"""{"Clusters": {"version": "$version"}}""")
      .flatMap(_ => getCluster(name))

  /**
   * Remove the specified cluster.
   *
   * @param name the cluster's name
   * @return the future of the cluster removal
   */
  def removeCluster(name: String): Future[Unit] =
    performRequest(baseUrl.DELETE / "clusters" / name).map(_ => ())

  private val bootstrapSequencer = new SequentialOperations

  def bootstrapMachines(hostnames: Set[String], sshKey: String): Future[Unit] = {
    val configuredBuilder = (baseUrl / "bootstrap").POST
      .setBody(compact(render(
        ("hosts" -> hostnames) ~
        ("sshKey" -> sshKey) ~
        ("verbose" -> true))))
      .addHeader("Content-Type", "application/json")
    def extractId(response: JValue) = response \ "status" match {
      case JString("OK") => {
        (response \ "requestId").extract[Int]
      }
      case JString("ERROR") => throw RequestException(
        configuredBuilder.build,
        s"Bootstrap request returned an error: ${response \\ "log"}")
      case _ => throw ServiceError(s"Unexpected Ambari response for bootstrap request: $response")
    }
    bootstrapSequencer enqueue {
      for {
        response <- performRequest(configuredBuilder)
        _ <- createRequestHandler(baseUrl / "bootstrap" / extractId(response)).ensureFinished
      } yield ()
    }
  }

  def registeredHostnames: Future[Set[String]] =
    performRequest(baseUrl / "hosts").map(json => as.FlatValues(json, "items", "host_name").toSet)
}
