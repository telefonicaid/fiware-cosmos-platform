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

/**
 * Representation of a server capable of provisioning clusters.
 */
private[ambari] trait ClusterProvisioner {
  /**
   * Get a list of the names of the existing, managed clusters.
   *
   * @return the future of the list of names
   */
  def listClusterNames: Future[Seq[String]]

  /**
   * Get the cluster specified by the given name.
   *
   * @param id the id of the cluster
   * @return the future of the cluster iff found
   */
  def getCluster(id: String): Future[Cluster]

  /**
   * Create a cluster.
   *
   * @param name the cluster's name
   * @param version the version of the Ambari Service stack, e.g. `"HDP-1.2.0"`
   * @return the future of the created cluster
   */
  def createCluster(name: String, version: String): Future[Cluster]

  /**
   * Remove the specified cluster.
   *
   * @param name the cluster's name
   * @return the future of the cluster removal
   */
  def removeCluster(name: String): Future[Unit]

  /**
   * Installs and launches Ambari agent
   */
  def bootstrapMachines(hostnames: Seq[String], sshKey: String): Future[Unit]

  /**
   * Stops and unregisters Ambari agent from Ambari server
   */
  def teardownMachines(hostnames: Seq[String], sshKey: String): Future[Unit]

  /**
   * Returns the list of hostnames that are registered in Ambari server
   */
  def registeredHostnames: Future[Seq[String]]
}
