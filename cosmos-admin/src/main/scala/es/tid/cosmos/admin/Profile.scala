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
    withProfile(handle) { (c, cosmosProfile) =>
      dao.setMachineQuota(cosmosProfile.id, quota)(c)
    }

  def enableCapability(handle: String, capability: Capability): Boolean = 
    withProfile(handle) { (c, cosmosProfile) =>
      dao.enableUserCapability(cosmosProfile.id, capability)(c)
    }

  def disableCapability(handle: String, capability: Capability): Boolean =
    withProfile(handle) { (c, cosmosProfile) =>
      dao.disableUserCapability(cosmosProfile.id, capability)(c)
    }

  private def withProfile(handle: String)
                         (action: (this.dao.type#Conn, CosmosProfile) => Unit): Boolean = {
    Try(dao.withTransaction { implicit c =>
      dao.lookupByHandle(handle) match {
        case None => {
          println(s"No user with handle $handle")
          false
        }
        case Some(cosmosProfile) => action(c, cosmosProfile)
      }
    }).isSuccess
  }

  def setGroup(handle: String, groupName: Option[String]): Boolean =
    withProfile(handle) { (c, cosmosProfile) =>
      dao.setGroup(cosmosProfile.id, groupName)(c)
    }
}
