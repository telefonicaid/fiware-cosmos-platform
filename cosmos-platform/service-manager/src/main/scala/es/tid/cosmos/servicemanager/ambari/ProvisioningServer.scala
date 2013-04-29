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
import net.liftweb.json.JsonAST.JValue

/**
 * Representation of a server capable of provisioning clusters.
 *
 * @author adamos
 */
trait ProvisioningServer {
  def listClusterNames: Future[Seq[String]]
  def getCluster(name: String): Future[Cluster]
  def createCluster(name: String, version: String): Future[Cluster]
  def removeCluster(name: String): Future[JValue]
}
