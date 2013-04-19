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

trait ServiceManager {
  def clusterIds: Seq[ClusterId]
  def createCluster(name: String, clusterSize: Int): ClusterId
  def describeCluster(id: ClusterId):  Option[ClusterDescription]
  def terminateCluster(id: ClusterId): Unit
}
