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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

/**
 * Provides up-to-date information on the state of a cluster.
 *
 * @constructor
 * @param id          Id of the cluster
 * @param name        Name of the cluster
 * @param size        Size of the cluster
 * @param deployment  Future that represents the deployment of the cluster
 */
class MutableClusterDescription(
    val id: ClusterId, val name: String, val size: Int, val deployment: Future[Any]) {

  def view: ClusterDescription = new ClusterDescription {
    override val state: ClusterState = MutableClusterDescription.this.state
    override val size: Int = MutableClusterDescription.this.size
    override val name: String = MutableClusterDescription.this.name
    override val id: ClusterId = MutableClusterDescription.this.id
  }

  @volatile var state: ClusterState = Provisioning

  deployment.onSuccess({case _ => state = Running})
  deployment.onFailure({case err => { state = new Failed(err) }})
}
