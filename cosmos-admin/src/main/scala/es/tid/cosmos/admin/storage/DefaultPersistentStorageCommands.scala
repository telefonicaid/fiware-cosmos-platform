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

package es.tid.cosmos.admin.storage

import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits._

import com.typesafe.config.Config

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

private[storage] class DefaultPersistentStorageCommands(
    serviceManager: ServiceManager, config: Config) extends PersistentStorageCommands {

  private val ClusterTimeout = 15.minutes

  override def setup(): CommandResult = storageState(serviceManager) match {
    case Some(Running) =>
      CommandResult.success("Infinity found. Not changing anything...")
    case Some(state) =>
      CommandResult.error(s"Infinity in $state state. Not changing anything...")
    case None =>
      println("Persistent Storage not found. Deploying...")
      val parameters = InfinityServerParameters(
        cosmosApiUrl = config.getString("application.baseurl"),
        infinitySecret = config.getString("infinity.secret")
      )
      val deployment = serviceManager.deployPersistentHdfsCluster(parameters)
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
