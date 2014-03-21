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

package es.tid.cosmos.admin.storage

import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits._

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters._

private[storage] class DefaultPersistentStorageCommands(serviceManager: ServiceManager)
  extends PersistentStorageCommands {

  private val ClusterTimeout = 15.minutes

  override def setup(): CommandResult = storageState(serviceManager) match {
    case Some(Running) =>
      CommandResult.success("Infinity found. Not changing anything...")
    case Some(state) =>
      CommandResult.error(s"Infinity in $state state. Not changing anything...")
    case None =>
      println("Persistent Storage not found. Deploying...")
      val deployment = serviceManager.deployPersistentHdfsCluster()
        .map(_ => CommandResult.success("Infinity successfully deployed"))
      CommandResult.await(deployment, ClusterTimeout)
  }

  override def terminate(): CommandResult = storageState(serviceManager) match {
    case Some(Running) =>
      println("Infinity found. Terminating...")
      val termination = serviceManager.terminatePersistentHdfsCluster()
        .map(_ => CommandResult.success("Infinity successfully terminated"))
      CommandResult.await(termination, ClusterTimeout)
    case Some(Provisioning) | Some(Failed(_)) =>
      CommandResult.error("Infinity cannot be terminated due its state.")
    case Some(Terminating) | Some(Terminated) =>
      CommandResult.success("Persistent Storage already terminated.")
    case None =>
      CommandResult.error("Persistent Storage not found. Cannot terminate...")
  }

  private def storageState(serviceManager: ServiceManager) =
    serviceManager.describePersistentHdfsCluster().map(_.state)
}
