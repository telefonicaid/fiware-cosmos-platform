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

package es.tid.cosmos.servicemanager.clusters

import java.net.URI

import es.tid.cosmos.servicemanager.ClusterUser

/**
 * A description of a cluster
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
   * The cluster's machines
   */
  final def machines: Seq[HostDetails] = (master ++ slaves).toSet.toSeq

  /**
   * Lifecycle state of the cluster.
   */
  def state: ClusterState

  /**
   * Name node URI with hdfs scheme.
   * For instance: hdfs://localhost:54310
   */
  def nameNode: Option[URI]

  /**
   * The cluster's master node information.
   * @return the master node information
   */
  def master: Option[HostDetails]

  /**
   * The cluster's slave nodes information.
   * @return the slave nodes information
   */
  def slaves: Seq[HostDetails]

  /** The users of this cluster. */
  def users: Option[Set[ClusterUser]]
}
