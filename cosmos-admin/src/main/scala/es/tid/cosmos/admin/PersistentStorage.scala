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

import es.tid.cosmos.servicemanager.ServiceManager

private[admin] object PersistentStorage {

  def setup(serviceManager: ServiceManager) = {
    if (hasPersistentStorage(serviceManager)) {
      println("Persistent Storage found. Not changing anything...")
      true
    } else {
      println("Persistent Storage not found. Deploying...")
      Util.waitUntilReady(serviceManager.deployPersistentHdfsCluster())
    }
  }

  def terminate(serviceManager: ServiceManager) = {
    if (hasPersistentStorage(serviceManager)) {
      println("Persistent Storage found. Terminating...")
      Util.waitUntilReady(serviceManager.terminatePersistentHdfsCluster())
    } else {
      println("Persistent Storage not found. Cannot terminate...")
      false
    }
  }

  private def hasPersistentStorage(serviceManager: ServiceManager) = {
    println("Checking if Persistent Storage cluster exists...")
    serviceManager.describePersistentHdfsCluster().isDefined
  }
}
