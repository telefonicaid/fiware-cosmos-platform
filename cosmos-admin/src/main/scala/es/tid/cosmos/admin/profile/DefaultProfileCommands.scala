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

import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.servicemanager.ServiceManager

private[profile] class DefaultProfileCommands(
    store: CosmosDataStore, serviceManager: ServiceManager) extends ProfileCommands {

  private lazy val listCommand = new ListProfilesCommand(store)

  override def list() = listCommand()
  override lazy val quota = new ProfileQuotaCommands(store)
  override lazy val capability = new ProfileCapabilityCommands(store)
  override lazy val group = new ProfileGroupCommands(store, serviceManager)
}
