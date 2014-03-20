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

package es.tid.cosmos.admin.profile

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
import es.tid.cosmos.servicemanager.{ClusterUser, ServiceManager}
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

  /** Represent a group change of a cluster user. */
  private case class GroupChange(user: ClusterUser, source: Group, target: Group)

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
        change <- requireValidGroupChange(handle, groupName)
        clusterUpdates = updateSharedClusters(change)
          .map(_ => CommandResult.success(s"User $handle now belongs to ${change.target}"))
      } yield CommandResult.await(clusterUpdates, ProfileCommands.ClusterUpdateTimeout)
    }

  /** Perform all group change validations on a single transaction. */
  private def requireValidGroupChange(
      handle: String, groupName: Option[String]): Validation[String, GroupChange] =
    store.withTransaction { implicit c =>
      for {
        cosmosProfile <- requireProfileWithHandle(handle)
        targetGroup <- if (groupName.isDefined) requireExistingGroup(groupName.get)
        else NoGroup.success
        _ <- requireNoActiveSharedClusterOwnedBy(cosmosProfile)
      } yield {
        store.profile.setGroup(cosmosProfile.id, targetGroup)
        val clusterUser = ClusterUser.enabled(
          username = cosmosProfile.handle,
          publicKey = cosmosProfile.keys.head.signature,
          isSudoer = cosmosProfile.capabilities.hasCapability(Capability.IsSudoer)
        )
        GroupChange(clusterUser, cosmosProfile.group, targetGroup)
      }
    }

  private def updateSharedClusters(change: GroupChange): Future[Unit] = {
    val handle = change.user.username
    val (failedDisables_>, failedAdditions_>) = store.withTransaction { implicit c =>
      (disableUserOnClusters(handle, clustersToLeave(handle, change.source)),
      enableUserOnClusters(change.user, clustersToJoin(change.target)))
    }
    for {
      failedDisables <- failedDisables_>
      failedAdditions <- failedAdditions_>
      failedUpdates = failedDisables ++ failedAdditions
    } yield require(
      failedUpdates.isEmpty,
      s"Cannot update user $handle successfully on clusters: " + failedUpdates.mkString(", ")
    )
  }

  private def enableUserOnClusters(user: ClusterUser, clusters: Set[ClusterId]) =
    modifyClusters(clusters) { clusterId =>
      serviceManager.addUser(clusterId, user)
    }

  private def disableUserOnClusters(handle: String, clusters: Set[ClusterId]) =
    modifyClusters(clusters) { clusterId =>
      serviceManager.disableUser(clusterId, handle)
    }

  /** Modify clusters in parallel.
    *
    * @param clusters   Identifiers of the clusters to operate on
    * @param operation  Operation to apply in parallel to every cluster
    * @return           The list of failed cluster identifiers
    */
  private def modifyClusters(clusters: Set[ClusterId])(operation: ClusterId => Future[_]) =
    Future.traverse(clusters) { clusterId: ClusterId =>
      operation(clusterId).map(_ => None).recover {
        case NonFatal(ex) => Some(clusterId)
      }
    }.map(_.flatten)

  private def clustersToLeave(handle: String, leftGroup: Group)
                             (implicit c: store.Conn): Set[ClusterId] = (for {
    description <- activeSharedClusters(leftGroup)
    users <- description.users if users.exists(_.username == handle)
  } yield description.id).toSet

  private def clustersToJoin(joinedGroup: Group)(implicit c: store.Conn): Set[ClusterId] =
    activeSharedClusters(joinedGroup).map(_.id).toSet

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
