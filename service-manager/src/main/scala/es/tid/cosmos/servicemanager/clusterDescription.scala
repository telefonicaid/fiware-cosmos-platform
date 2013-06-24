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

package es.tid.cosmos.servicemanager

import java.net.URI
import scala.concurrent.Future

/**
 * An immutable description of a cluster
 */
trait ClusterDescription {

  /**
   * Unique cluster identifier.
   */
  def id: ClusterId

  /**
   * Human-readable name of the cluster.
   */
  def name: String

  /**
   * Number of machines.
   */
  def size: Int

  /**
   * Lifecycle state of the cluster.
   */
  def state: ClusterState

  /**
   * Name node URI with hdfs scheme.
   * For instance: hdfs://localhost:54310
   */
  def nameNode_> : Future[URI]

  /**
   * The cluster's master node information.
   * @return the master node information
   */
  def master_> : Future[HostDetails]

  /**
   * The cluster's slave nodes information.
   * @return the slave nodes information
   */
  def slaves_> : Future[Seq[HostDetails]]
}

/**
 * Representation of the basic information of a host.
 *
 * @param hostname the host's name
 * @param ipAddress the host's IP address
 */
case class HostDetails(hostname: String, ipAddress: String)
