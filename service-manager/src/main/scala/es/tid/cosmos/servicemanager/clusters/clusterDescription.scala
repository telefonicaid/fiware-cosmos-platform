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
}

/**
 * An immutable description of a cluster
 */
case class ImmutableClusterDescription(
  override val id: ClusterId,
  override val name: String,
  override val size: Int,
  override val state: ClusterState,
  override val nameNode: Option[URI],
  override val master: Option[HostDetails],
  override val slaves: Seq[HostDetails]) extends ClusterDescription

/**
 * An mutable description of a cluster that includes setters
 */
trait MutableClusterDescription extends ClusterDescription {
  override val id: ClusterId
  override def name: String
  def name_=(name: String): Unit
  override def state: ClusterState
  def state_=(state: ClusterState): Unit
  override def nameNode: Option[URI]
  def nameNode_=(nameNode: URI): Unit
  override def master: Option[HostDetails]
  def master_=(master: HostDetails): Unit
  override def slaves: Seq[HostDetails]
  def slaves_=(slaves: Seq[HostDetails]): Unit

  final def view: ImmutableClusterDescription = new ImmutableClusterDescription(
    id, name, size, state, nameNode, master, slaves)
}

/**
 * Representation of the basic information of a host.
 *
 * @param hostname the host's name
 * @param ipAddress the host's IP address
 */
case class HostDetails(hostname: String, ipAddress: String)
