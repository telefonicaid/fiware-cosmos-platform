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

import scala.util.Try

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Capability.Capability

private[admin] class Profile(dao: CosmosProfileDao) {

  def setMachineQuota(handle: String, limit: Int): Boolean = setMachineQuota(handle, Quota(limit))

  def removeMachineQuota(handle: String): Boolean = setMachineQuota(handle, UnlimitedQuota)

  private def setMachineQuota(handle: String, quota: Quota): Boolean =
    dao.withTransaction { implicit c =>
      for {
        cosmosProfile <- withProfile(handle)
        success <- Try(dao.setMachineQuota(cosmosProfile.id, quota)).toOption
      } yield success
    }.isDefined

  def enableCapability(handle: String, capability: String): Boolean =
    modifyCapability(handle, capability, enable = true)

  def disableCapability(handle: String, capability: String): Boolean =
    modifyCapability(handle, capability, enable = false)

  private def modifyCapability(handle: String, capability: String, enable: Boolean): Boolean =
    dao.withTransaction { implicit c =>
      val action = if (enable) dao.enableUserCapability _ else dao.disableUserCapability _
      for {
        cosmosProfile <- withProfile(handle)
        parsedCapability <- parseCapability(capability)
        success <- Try(action(cosmosProfile.id, parsedCapability)).toOption
      } yield success
    }.isDefined

  def setGroup(handle: String, groupName: Option[String]): Boolean =
    dao.withTransaction { implicit c =>
      for {
        cosmosProfile <- withProfile(handle)
        success <- Try(dao.setGroup(cosmosProfile.id, groupName)).toOption
      } yield success
    }.isDefined

  private def withProfile(handle: String)
                         (implicit c: this.dao.type#Conn): Option[CosmosProfile] =
    whenEmpty(dao.lookupByHandle(handle)) {
      println(s"No user with handle $handle")
    }

  private def parseCapability(input: String): Option[Capability] =
    whenEmpty(Capability.values.find(_.toString == input)) {
      println(s"Unknown capability '$input', one of ${Capability.values.mkString(", ")} was expected")
    }

  private def whenEmpty[T](value: Option[T])(action: => Unit): Option[T] = {
    if (value.isEmpty) action
    value
  }
}
