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

import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

trait DefaultGroupCommandsComponent extends GroupCommandsComponent {
  this: CosmosDataStoreComponent with ServiceManagerComponent =>

  override lazy val groupCommands: GroupCommands = new DefaultGroupCommands(store, serviceManager)
}
