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
import es.tid.cosmos.servicemanager.clusters._

private[admin] object PersistentStorage {

  def setup(serviceManager: ServiceManager) = {
    storageState(serviceManager) match {
      case Some(Running) =>
        println("Persistent Storage found. Not changing anything...")
        true
      case Some(_) =>
        println("Persistent Storage Terminated. Not changing anything...")
        false
      case None =>
        println("Persistent Storage not found. Deploying...")
        Util.waitUntilReady(serviceManager.deployPersistentHdfsCluster())
    }
  }

  def terminate(serviceManager: ServiceManager) = {
    storageState(serviceManager) match {
      case Some(Running) =>
        println("Persistent Storage found. Terminating...")
        Util.waitUntilReady(serviceManager.terminatePersistentHdfsCluster())
      case Some(Provisioning) | Some(Failed(_)) =>
        println("Persistent Storage cannot be terminated due its state.")
        false
      case Some(Terminating) | Some(Terminated) =>
        println("Persistent Storage already terminated.")
        true
      case None =>
        println("Persistent Storage not found. Cannot terminate...")
        false
    }
  }

  private def storageState(serviceManager: ServiceManager) =
    serviceManager.describePersistentHdfsCluster().map(_.state)
}
