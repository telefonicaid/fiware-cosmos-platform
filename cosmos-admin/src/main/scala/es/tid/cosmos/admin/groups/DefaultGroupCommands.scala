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

package es.tid.cosmos.admin.groups

import scalaz.{Scalaz, Validation}

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.admin.validation.GroupValidations
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, GroupDataStore, ProfileDataStore}
import es.tid.cosmos.api.quota._
import es.tid.cosmos.api.usage.DynamicMachineUsage
import es.tid.cosmos.servicemanager.ServiceManager

/** Admin commands for managing groups. */
private[groups] class DefaultGroupCommands(
    override val store: ProfileDataStore with GroupDataStore with ClusterDataStore,
    serviceManager: ServiceManager) extends GroupCommands with GroupValidations {
  import Scalaz._

  private val quotaContext = new QuotaContextFactory(new DynamicMachineUsage(store, serviceManager))

  override def create(name: String, minQuota: Int): CommandResult = CommandResult.fromValidation {
    val group = GuaranteedGroup(name, Quota(minQuota))
    for {
      _ <- requireFeasibleQuota(group)
      _ <- Validation.fromTryCatch(store.withTransaction { implicit c =>
        store.group.register(group)
      }).leftMap(_.getMessage)
    } yield CommandResult.success(s"Group $group created successfully")
  }

  override def list(): CommandResult = CommandResult.success {
    val groups = store.withConnection { implicit c =>
      store.group.list() - NoGroup
    }
    if (groups.isEmpty) "No groups available"
    else s"Available groups: [Name | Minimum Quota]:\n${groups.map(toUserFriendly).mkString("\n")}"
  }

  private def toUserFriendly(group: Group): String = s"${group.name} | ${group.minimumQuota}"

  override def delete(name: String): CommandResult = CommandResult.fromValidation(
    store.withTransaction { implicit c =>
      for {
        group <- requireExistingGroup(name)
        _ <- requireNoSharedCluster(group)
      } yield {
        store.group.delete(group.name)
        CommandResult.success(s"Group $name successfully deleted")
      }
    })

  private def requireNoSharedCluster(group: GuaranteedGroup)
                                    (implicit c: store.Conn): Validation[String, Unit] = {
    val runningSharedClusters = for {
      clusterId <- serviceManager.clusterIds
      clusterDescription <- serviceManager.describeCluster(clusterId)
      if clusterDescription.state.isActive
      cluster = store.cluster.get(clusterId) if cluster.shared
      owner <- store.profile.lookupByProfileId(cluster.ownerId) if owner.group == group
    } yield clusterId

    if (runningSharedClusters.isEmpty) ().success
    else (s"Cannot delete group ${group.name}: " +
      s"there are running shared clusters (${runningSharedClusters.mkString(", ")})").fail
  }

  override def setMinQuota(name: String, quota: Int): CommandResult = CommandResult.fromValidation {
    store.withTransaction { implicit c =>
      for {
        group <- requireExistingGroup(name)
        _ <- requireFeasibleQuota(group.copy(minimumQuota = Quota(quota)))
      } yield {
        store.group.setQuota(group.name, Quota(quota))
        CommandResult.success(s"Quota for group $name changed to $quota")
      }
    }
  }

  private def requireFeasibleQuota(group: GuaranteedGroup): Validation[String, Unit] =
    quotaContext().isGroupQuotaFeasible(group)
}
