/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.admin.cluster

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.ClusterId

private[cluster] class DefaultClusterCommands(serviceManager: ServiceManager)
  extends ClusterCommands {

  private val commandTimeout = 15.minutes

  override def terminate(clusterId: ClusterId): CommandResult =
    serviceManager.describeCluster(clusterId) match {
      case None =>
        CommandResult.error("Cluster not found. Cannot terminate...")
      case _ =>
        println("Cluster found. Terminating...")
        val clusterTermination = serviceManager.terminateCluster(clusterId)
          .map(_ => CommandResult.success("Cluster terminated successfully"))
        CommandResult.await(clusterTermination, commandTimeout)
    }
}
