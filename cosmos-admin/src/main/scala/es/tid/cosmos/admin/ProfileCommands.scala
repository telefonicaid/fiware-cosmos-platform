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

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.util.control.NonFatal
import scalaz.{Scalaz, Validation}

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.admin.validation.GroupChecks
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Capability.Capability
import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.api.quota.{Group, NoGroup, UnlimitedQuota, Quota}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ClusterId}

private[admin] class ProfileCommands(
    override val store: CosmosDataStore, serviceManager: ServiceManager) extends GroupChecks {

  import Scalaz._

  def setMachineQuota(handle: String, limit: Int): CommandResult =
    setMachineQuota(handle, Quota(limit))

  def removeMachineQuota(handle: String): CommandResult = setMachineQuota(handle, UnlimitedQuota)

  private def setMachineQuota(handle: String, quota: Quota) =
    transactionalValidationCommand { implicit c =>
      for {
        cosmosProfile <- requireProfileWithHandle(handle)
        _ <- Validation.fromTryCatch(
          store.profile.setMachineQuota(cosmosProfile.id, quota)
        ).leftMap(_.getMessage)
      } yield CommandResult.success(s"Machine quota for user $handle changed to $quota")
    }

  def enableCapability(handle: String, capability: String): CommandResult =
    modifyCapability(handle, capability, enable = true)

  def disableCapability(handle: String, capability: String): CommandResult =
    modifyCapability(handle, capability, enable = false)

  def setGroup(handle: String, groupName: String): CommandResult =
    changeToGroup(handle, Some(groupName))

  def removeGroup(handle: String): CommandResult = changeToGroup(handle, groupName = None)

  def list(): CommandResult = store.withTransaction { implicit c =>
    val handles = for (
      profile <- store.profile.list() if profile.state != UserState.Deleted
    ) yield profile.handle
    CommandResult.success(
      if (handles.isEmpty) "No users found"
      else s"Users found (handles):\n${handles.sorted.mkString("\n")}"
    )
  }

  private def modifyCapability(handle: String, capability: String, enable: Boolean): CommandResult =
    transactionalValidationCommand { implicit c =>
      val (verb, action) =
        if (enable) ("enabled", store.capability.enable _)
        else ("disabled", store.capability.disable _)
      for {
        cosmosProfile <- requireProfileWithHandle(handle)
        parsedCapability <- requireValidCapability(capability)
      } yield {
        action(cosmosProfile.id, parsedCapability)
        CommandResult.success(s"$parsedCapability $verb for user $handle")
      }
    }

  private def requireValidCapability(input: String): Validation[String, Capability] =
    Capability.values.find(_.toString == input).toSuccess(
      s"Unknown capability '$input', one of ${Capability.values.mkString(", ")} was expected"
    )

  /** Handle setting or removing a group from a user based on the `groupName` option.
    *
    * @param handle the user's handle
    * @param groupName the optional group name. If a name is provided then the user will be set
    *                  to that group. If `None` is provided then this will be interpreted as removing
    *                  the user from any group that they might have belonged to.
    * @return          true iff the operation was successful
    */
  private def changeToGroup(handle: String, groupName: Option[String]): CommandResult =
    CommandResult.fromValidation {
      for {
        groupChange <- requireGroupChange(handle, groupName)
        (sourceGroup, targetGroup) = groupChange
        clusterUpdates = removeFromSharedClusters(handle, sourceGroup)
          .map(_ => CommandResult.success(s"User $handle now belongs to $targetGroup"))
      } yield CommandResult.await(clusterUpdates, ProfileCommands.ClusterUpdateTimeout)
    }

  private def requireGroupChange(
      handle: String, groupName: Option[String]): Validation[String, (Group, Group)] =
    store.withTransaction { implicit c =>
      for {
        cosmosProfile <- requireProfileWithHandle(handle)
        targetGroup <- if (groupName.isDefined) requireExistingGroup(groupName.get)
        else NoGroup.success
        _ <- requireNoActiveSharedClusterOwnedBy(cosmosProfile)
      } yield {
        store.profile.setGroup(cosmosProfile.id, targetGroup)
        (cosmosProfile.group, targetGroup)
      }
    }

  private def removeFromSharedClusters(handle: String, group: Group): Future[Unit] = {
    val clustersToUpdate = store.withTransaction { implicit c =>
      for {
        description <- activeSharedClusters(group)
        users <- description.users if users.exists(_.username == handle)
      } yield description.id
    }
    for {
      updateResults <- Future.traverse(clustersToUpdate) { clusterId: ClusterId =>
        serviceManager.disableUser(clusterId, handle)
          .map(_ => None)
          .recover { case NonFatal(ex) => Some(clusterId) }
      }
      failedUpdates = updateResults.flatten
    } yield require(
      failedUpdates.isEmpty,
      s"Cannot remove user $handle from: ${failedUpdates.mkString(", ")}"
    )
  }

  private def activeSharedClusters(group: Group)(implicit c: store.Conn): Seq[ClusterDescription] =
    for {
      clusterId <- serviceManager.clusterIds
      ownerId <- store.cluster.ownerOf(clusterId)
      owner <- store.profile.lookupByProfileId(ownerId) if owner.group == group
      description <- serviceManager.describeCluster(clusterId) if description.state.isActive
    } yield description

  private def requireProfileWithHandle(handle: String)
                                      (implicit c: store.Conn): Validation[String, CosmosProfile] =
    store.profile.lookupByHandle(handle).toSuccess(s"No user with handle $handle")

  private def requireNoActiveSharedClusterOwnedBy(
      profile: CosmosProfile)(implicit c: store.Conn): Validation[String, Unit] = {
    val offendingClusters = for {
      cluster <- store.cluster.ownedBy(profile.id)
      description <- serviceManager.describeCluster(cluster.clusterId)
      if description.state.isActive
    } yield cluster.clusterId
    if (offendingClusters.isEmpty) ().success
    else s"User ${profile.handle} owns shared clusters: ${offendingClusters.mkString(", ")}".fail
  }


  private def transactionalValidationCommand(
      command: store.Conn => Validation[String, CommandResult]): CommandResult =
    CommandResult.fromValidation(store.withTransaction(command))
}

object ProfileCommands {
  val ClusterUpdateTimeout = 20.minutes
}
