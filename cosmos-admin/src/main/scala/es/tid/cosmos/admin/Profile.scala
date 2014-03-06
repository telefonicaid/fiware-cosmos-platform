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


import es.tid.cosmos.admin.Util._
import es.tid.cosmos.admin.validation.GroupChecks
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Capability.Capability
import es.tid.cosmos.api.quota.{UnlimitedQuota, Quota}

private[admin] class Profile(override val dao: CosmosDao) extends GroupChecks {

  def setMachineQuota(handle: String, limit: Int): Boolean = setMachineQuota(handle, Quota(limit))

  def removeMachineQuota(handle: String): Boolean = setMachineQuota(handle, UnlimitedQuota)

  private def setMachineQuota(handle: String, quota: Quota): Boolean =
    dao.withTransaction { implicit c =>  tryAction {
      for {
        cosmosProfile <- withProfile(handle)
      } yield {
        dao.profile.setMachineQuota(cosmosProfile.id, quota)
        println(s"Machine quota for user $handle changed to $quota")
      }
    }}

  def enableCapability(handle: String, capability: String): Boolean =
    modifyCapability(handle, capability, enable = true)

  def disableCapability(handle: String, capability: String): Boolean =
    modifyCapability(handle, capability, enable = false)

  def setGroup(handle: String, groupName: String): Boolean = handleGroup(handle, Some(groupName))

  def removeGroup(handle: String): Boolean = handleGroup(handle, groupName = None)

  def list: String = dao.withTransaction { implicit c =>
    val handles = for (
      profile <- dao.profile.list() if profile.state != UserState.Deleted
    ) yield profile.handle
    if (handles.isEmpty) "No users found"
    else s"Users found (handles):\n${handles.sorted.mkString("\n")}"
  }

  private def withProfile(handle: String)(implicit c: this.dao.type#Conn): Option[CosmosProfile] =
    whenEmpty(dao.profile.lookupByHandle(handle)) {
      println(s"No user with handle $handle")
    }

  private def modifyCapability(handle: String, capability: String, enable: Boolean): Boolean =
    dao.withTransaction { implicit c => tryAction {
      val action = if (enable) dao.capability.enable _ else dao.capability.disable _
      for {
        cosmosProfile <- withProfile(handle)
        parsedCapability <- parseCapability(capability)
      } yield {
        action(cosmosProfile.id, parsedCapability)
        val verb = if (enable) "enabled" else "disabled"
        println(s"$parsedCapability $verb for user $handle")
      }
    }}

  private def parseCapability(input: String): Option[Capability] =
    whenEmpty(Capability.values.find(_.toString == input)) {
      println(s"Unknown capability '$input', one of ${Capability.values.mkString(", ")} was expected")
    }

  /**
    * Handle setting or removing a group from a user based on the `groupName` option.
    *
    * @param handle the user's handle
    * @param groupName the optional group name. If a name is provided then the user will be set
    *                  to that group. If `None` is provided then this will be interpreted as removing
    *                  the user from any group that they might have belonged to.
    * @return          true iff the operation was successful
    */
  private def handleGroup(handle: String, groupName: Option[String]): Boolean =
    dao.withTransaction { implicit c => tryAction {
      val maybeGroupName = groupName.flatMap(withGroup).map(_.name)
      val groupNameGivenButNotFound = groupName.isDefined && maybeGroupName.isEmpty
      if (groupNameGivenButNotFound)
        None
      else for {
        cosmosProfile <- withProfile(handle)
      } yield {
        dao.profile.setGroup(cosmosProfile.id, maybeGroupName)
        println(s"User $handle now belongs to group $groupName")
      }
    }}
}
