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

package es.tid.cosmos.admin.groups

import scalaz.{Scalaz, Validation}

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.admin.validation.GroupChecks
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, GroupDataStore, ProfileDataStore}
import es.tid.cosmos.api.quota._
import es.tid.cosmos.api.usage.DynamicMachineUsage
import es.tid.cosmos.servicemanager.ServiceManager

/** Admin commands for managing groups. */
private[admin] class GroupCommands(
    override val store: ProfileDataStore with GroupDataStore with ClusterDataStore,
    serviceManager: ServiceManager) extends GroupChecks {
  import Scalaz._
  import GroupCommands._

  private val quotaContext = new QuotaContextFactory(new DynamicMachineUsage(store, serviceManager))

  /** Create a group.
    *
    * @param name     the name of the group
    * @param minQuota the group's minimum, guaranteed quota
    * @return         true iff the group was successfully created
    */
  def create(name: String, minQuota: Int): CommandResult = CommandResult.fromValidation {
    val group = GuaranteedGroup(name, Quota(minQuota))
    for {
      _ <- requireFeasibleQuota(group)
      _ <- Validation.fromTryCatch(store.withTransaction { implicit c =>
        store.group.register(group)
      }).leftMap(_.getMessage)
    } yield CommandResult.success(s"Group $group created successfully")
  }

  /** List the existing groups.
    *
    * @return all the existing groups filtering out the implied
    *         [[es.tid.cosmos.api.quota.NoGroup]]
    */
  def list(): CommandResult = CommandResult.success {
    val groups = store.withConnection { implicit c =>
      store.group.list() - NoGroup
    }
    if (groups.isEmpty) "No groups available"
    else s"Available groups: [Name | Minimum Quota]:\n${groups.map(toUserFriendly).mkString("\n")}"
  }

  /** Delete an existing group.
    *
    * @param name the group's name
    * @return     whether the group was successfully deleted
    */
  def delete(name: String): CommandResult = CommandResult.fromValidation(
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

  /** Set an existing group's minimum, guaranteed quota.
    *
    * @param name  the name of the group
    * @param quota the new minimum quota
    * @return      true if the group was successfully updated with the new minimum quota
    */
  def setMinQuota(name: String, quota: Int): CommandResult = CommandResult.fromValidation {
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

private object GroupCommands {
  private def toUserFriendly(group: Group): String = s"${group.name} | ${group.minimumQuota}"
}
