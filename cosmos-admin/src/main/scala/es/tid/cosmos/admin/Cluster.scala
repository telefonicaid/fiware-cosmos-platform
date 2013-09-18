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

package es.tid.cosmos.admin

import es.tid.cosmos.servicemanager.{ServiceManager, ClusterId}

private[admin] object Cluster {

  def terminate(serviceManager: ServiceManager, clusterId: ClusterId) = {
    serviceManager.describeCluster(clusterId) match {
      case None => {
        println("Cluster not found. Cannot terminate...")
        false
      }
      case _ => {
        println("Cluster found. Terminating...")
        Util.waitUntilReady(serviceManager.terminateCluster(clusterId))
      }
    }
  }
}
