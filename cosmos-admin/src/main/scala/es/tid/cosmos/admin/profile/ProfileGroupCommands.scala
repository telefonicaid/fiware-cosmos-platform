package es.tid.cosmos.admin.profile

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.util.control.NonFatal
import scalaz.{Scalaz, Validation}

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.admin.validation.GroupValidations
import es.tid.cosmos.api.profile.{CosmosProfile, Capability}
import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.api.quota.{NoGroup, Group}
import es.tid.cosmos.servicemanager.{ServiceManager, ClusterUser}
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ClusterId}

class ProfileGroupCommands(override val store: CosmosDataStore, serviceManager: ServiceManager)
    extends ProfileCommands.GroupCommands with ProfileValidations with GroupValidations {
  import Scalaz._

  override def set(handle: String, groupName: String): CommandResult =
    changeToGroup(handle, Some(groupName))

  override def remove(handle: String): CommandResult = changeToGroup(handle, groupName = None)

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
        infinityUpdate = updateInfinity(change)
        allUpdates = Future.sequence(Seq(clusterUpdates, infinityUpdate))
          .map(_ => CommandResult.success(s"User $handle now belongs to ${change.target}"))
      } yield CommandResult.await(allUpdates, ProfileGroupCommands.ClusterUpdateTimeout)
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
          group = cosmosProfile.group.hdfsGroupName,
          publicKey = cosmosProfile.keys.head.signature,
          isSudoer = cosmosProfile.capabilities.hasCapability(Capability.IsSudoer)
        )
        GroupChange(clusterUser, cosmosProfile.group, targetGroup)
      }
    }

  private def requireNoActiveSharedClusterOwnedBy(
      profile: CosmosProfile)(implicit c: store.Conn): Validation[String, Unit] = {
    val offendingClusters = activeSharedClustersOwnedBy(profile)
    if (offendingClusters.isEmpty) ().success
    else s"User ${profile.handle} owns shared clusters: ${offendingClusters.mkString(", ")}".fail
  }

  private def activeSharedClustersOwnedBy(
      profile: CosmosProfile)(implicit c: store.Conn): Seq[ClusterId] = for {
    cluster <- store.cluster.ownedBy(profile.id)
    if cluster.shared
    description <- serviceManager.describeCluster(cluster.clusterId)
    if description.state.isActive
  } yield cluster.clusterId

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

  private def updateInfinity(change: GroupChange): Future[Unit] = {
    val oldUsers = serviceManager.describePersistentHdfsCluster().get.users.get
    val user  = oldUsers.find(_.username == change.user.username).get
    val newUsers = (oldUsers - user) + user.copy(group = change.target.hdfsGroupName)
    serviceManager.setUsers(serviceManager.persistentHdfsId, newUsers.toSeq)
  }

  private def clustersToLeave(handle: String, leftGroup: Group)
                             (implicit c: store.Conn): Set[ClusterId] = for {
    description <- activeSharedClusters(leftGroup)
    users <- description.users if users.exists(_.username == handle)
  } yield description.id

  private def clustersToJoin(joinedGroup: Group)(implicit c: store.Conn): Set[ClusterId] =
    activeSharedClusters(joinedGroup).map(_.id)

  private def activeSharedClusters(group: Group)(implicit c: store.Conn): Set[ClusterDescription] =
    (for {
      clusterId <- serviceManager.clusterIds
      ownerId <- store.cluster.ownerOf(clusterId)
      owner <- store.profile.lookupByProfileId(ownerId) if owner.group == group
      description <- serviceManager.describeCluster(clusterId) if description.state.isActive
    } yield description).toSet

  private def enableUserOnClusters(user: ClusterUser, clusters: Set[ClusterId]) =
    modifyClustersInParallel(clusters) { clusterId =>
      serviceManager.addUser(clusterId, user)
    }

  private def disableUserOnClusters(handle: String, clusters: Set[ClusterId]) =
    modifyClustersInParallel(clusters) { clusterId =>
      serviceManager.disableUser(clusterId, handle)
    }

  /** Modify clusters in parallel.
    *
    * @param clusters   Identifiers of the clusters to operate on
    * @param operation  Operation to apply in parallel to every cluster
    * @return           The list of failed cluster identifiers
    */
  private def modifyClustersInParallel(clusters: Set[ClusterId])
                                      (operation: ClusterId => Future[_]) =
    Future.traverse(clusters) { clusterId: ClusterId =>
      operation(clusterId).map(_ => None).recover {
        case NonFatal(ex) => Some(clusterId)
      }
    }.map(_.flatten)
}

object ProfileGroupCommands {
  val ClusterUpdateTimeout = 20.minutes
}
