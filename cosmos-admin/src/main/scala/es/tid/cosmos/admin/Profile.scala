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

private[admin] class Profile(override val dao: CosmosProfileDao) extends GroupChecks {

  def setMachineQuota(handle: String, limit: Int): Boolean = setMachineQuota(handle, Quota(limit))

  def removeMachineQuota(handle: String): Boolean = setMachineQuota(handle, UnlimitedQuota)

  private def setMachineQuota(handle: String, quota: Quota): Boolean =
    dao.withTransaction { implicit c =>  tryAction {
      for {
        cosmosProfile <- withProfile(handle)
      } yield {
        dao.setMachineQuota(cosmosProfile.id, quota)
        println(s"Machine quota for user $handle changed to $quota")
      }
    }}

  def enableCapability(handle: String, capability: String): Boolean =
    modifyCapability(handle, capability, enable = true)

  def disableCapability(handle: String, capability: String): Boolean =
    modifyCapability(handle, capability, enable = false)

  def setGroup(handle: String, groupName: Option[String]): Boolean =
    dao.withTransaction { implicit c => tryAction {
      for {
        cosmosProfile <- withProfile(handle)
        name <- groupName
        group <- withGroup(name)
      } yield {
        dao.setGroup(cosmosProfile.id, Some(group.name))
        println(s"User $handle now belongs to group $groupName")
      }
    }}

  def removeGroup(handle: String): Boolean =
    dao.withTransaction { implicit c => tryAction {
      for {
        cosmosProfile <- withProfile(handle)
      } yield {
        dao.setGroup(cosmosProfile.id, None)
        println(s"User $handle was removed from group")
      }
    }}

  def list: String = dao.withTransaction { implicit c =>
    val handles = dao.getAllUsers().map(_.handle).sorted
    if (handles.isEmpty) "No users found"
    else s"Users found (handles):\n${handles.mkString(", ")}"
  }

  private def withProfile(handle: String)
                         (implicit c: this.dao.type#Conn): Option[CosmosProfile] =
    whenEmpty(dao.lookupByHandle(handle)) {
      println(s"No user with handle $handle")
    }

  private def modifyCapability(handle: String, capability: String, enable: Boolean): Boolean =
    dao.withTransaction { implicit c => tryAction {
      val action = if (enable) dao.enableUserCapability _ else dao.disableUserCapability _
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
}
