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
  def nameNode: URI
}
