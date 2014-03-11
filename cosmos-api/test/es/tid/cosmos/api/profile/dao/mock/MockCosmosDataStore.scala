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

package es.tid.cosmos.api.profile.dao.mock

import scala.concurrent.stm._

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.api.profile.dao._
import es.tid.cosmos.api.quota._
import es.tid.cosmos.servicemanager.clusters.ClusterId

class MockCosmosDataStore extends CosmosDataStore {

  private val users = Ref(Map[UserId, CosmosProfile]())
  private val clusters = Ref(List[ClusterAssignment]())
  private val groups = Ref(Set(Group.noGroup))
  private val bannedStates = Ref(Set.empty[UserState])

  def throwOnUserStateChangeTo(state: UserState): Unit = atomic { implicit txn =>
    bannedStates() = bannedStates() + state
  }

  override type Conn = InTxn
  override def withConnection[A](block: (Conn) => A): A = withTransaction(block)
  override def withTransaction[A](block: (Conn) => A): A = atomic { implicit txn => block(txn) }

  override def profile = new ProfileDao[Conn] {

    override def register(userId: UserId, reg: Registration, state: UserState)
                         (implicit c: Conn): CosmosProfile = {
      val cosmosProfile = CosmosProfile(
        id = users().size,
        state = state,
        handle = reg.handle,
        email = reg.email,
        apiCredentials = ApiCredentials.random(),
        keys = List(NamedKey("default", reg.publicKey))
      )
      require(!users().values.exists(_.handle == reg.handle), s"Duplicated handle: ${reg.handle}")
      require(groups().contains(cosmosProfile.group), s"Group not registered: ${cosmosProfile.group}")
      users() = users().updated(userId, cosmosProfile)
      cosmosProfile
    }

    override def list()(implicit c: Conn): Seq[CosmosProfile] = users().values.toSeq

    override def lookupByProfileId(id: ProfileId)(implicit c: Conn): Option[CosmosProfile] =
      users().values.find(_.id == id)

    override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
      users().get(userId)

    override def lookupByApiCredentials(creds: ApiCredentials)
                                       (implicit c: Conn): Option[CosmosProfile] =
      users().collectFirst {
        case (_, profile) if profile.apiCredentials == creds => profile
      }

    override def lookupByHandle(handle: String)(implicit c: Conn): Option[CosmosProfile] =
      users().values.find(_.handle == handle)

    override def lookupByGroup(group: Group)(implicit c: Conn): Set[CosmosProfile] =
      users().values.filter(profile => profile.group == group).toSet

    override def handleExists(handle: String)(implicit c: Conn): Boolean =
      users().values.exists(_.handle == handle)

    override def setHandle(id: ProfileId, handle: String)(implicit c: Conn) =
      updateProfile(id) { profile =>
        if (profile.handle != handle && users().values.exists(_.handle == handle))
          throw CosmosDaoException.duplicatedHandle(handle)
        profile.copy(handle = handle)
      }

    override def setEmail(id: ProfileId, email: String)(implicit c: Conn) =
      updateProfile(id) { profile =>
        profile.copy(email = email)
      }

    override def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn) =
      updateProfile(id) { profile =>
        profile.copy(keys = publicKeys)
      }

    override def setUserState(id: ProfileId, userState: UserState)(implicit c: Conn) =
      if (bannedStates().contains(userState)) {
        throw new RuntimeException("Forced failure: cannot change user state")
      } else updateProfile(id) { profile =>
        profile.copy(state = userState)
      }

    override def setGroup(id: ProfileId, groupName: Option[String])(implicit c: Conn) = {
      val maybeGroup = if (groupName.isEmpty) Some(NoGroup) else groupName.flatMap(groupByName)
      for (group <- maybeGroup) yield {
        updateProfile(id)(profile => profile.copy(group = group))
      }
    }

    override def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Conn) =
      updateProfile(id) { profile => profile.copy(quota = quota) }
  }

  override def capability = new CapabilityDao[Conn] {

    override def enable(id: ProfileId, capability: Capability.Value)(implicit c: Conn) =
      updateProfile(id) { profile =>
        profile.copy(capabilities = profile.capabilities + capability)
      }

    override def disable(id: ProfileId, capability: Capability.Value)(implicit c: Conn) =
      updateProfile(id) { profile =>
        profile.copy(capabilities = profile.capabilities - capability)
      }

    override def userCapabilities(id: ProfileId)(implicit c: Conn): UserCapabilities =
      profile.lookupByProfileId(id).map(_.capabilities).getOrElse(UntrustedUserCapabilities)
  }

  override def group = new GroupDao[Conn] {

    override def register(group: Group)(implicit c: Conn): Unit = {
      groups() = groups() + group
    }

    override def delete(name: String)(implicit c: Conn): Unit = {
      val groupUsers = users().values.filter(profile => profile.group.name == name)
      groupUsers.foreach { profile =>
        updateProfile(profile.id) { profile => profile.copy(group = NoGroup) }
      }
      groups() = groups().filterNot(_.name == name)
    }

    override def list()(implicit c: Conn): Set[Group] = groups()

    override def setQuota(name: String, minQuota: LimitedQuota)(implicit c: Conn): Unit = {
      val updated = GuaranteedGroup(name, minQuota)
      updateUsersWithGroup(updated)
      groups() = groups().filterNot(_.name == name) + updated
    }
  }

  override def cluster = new ClusterDao[Conn] {

    override def ownedBy(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment] =
      clusters().filter(_.ownerId == id)

    override def ownerOf(clusterId: ClusterId)(implicit c: Conn): Option[ProfileId] =
      clusters().find(_.clusterId == clusterId).map(_.ownerId)

    override def register(assignment: ClusterAssignment)(implicit c: Conn): Unit = {
      require(!clusters().exists(_.clusterId == assignment.clusterId), "Cluster already assigned")
      clusters() = clusters() :+ assignment
    }
  }

  private def updateProfile(id: ProfileId)(f: CosmosProfile => CosmosProfile): Unit =
    atomic { implicit txn =>
      val updatedUserId = for {
        (userId, profile) <- users() if profile.id == id
      } yield {
        users() = users().updated(userId, f(profile))
        userId
      }
      if (updatedUserId.isEmpty) throw CosmosDaoException.unknownUser(id)
    }

  private def groupByName(name: String): Option[Group] = atomic { implicit txn =>
    require(groups().exists(_.name == name), s"Unregistered group: $name")
    groups().find(_.name == name)
  }

  private def updateUsersWithGroup(group: Group) = atomic { implicit txn =>
    for ((userId, profile) <- users() if profile.group.name == group.name) {
      users() = users().updated(userId, profile.copy(group = group))
    }
  }
}
