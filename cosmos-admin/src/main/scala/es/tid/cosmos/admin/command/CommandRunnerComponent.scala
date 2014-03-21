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

package es.tid.cosmos.admin.command

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.cluster.ClusterCommandsComponent
import es.tid.cosmos.admin.profile.ProfileCommandsComponent
import es.tid.cosmos.admin.storage.PersistentStorageCommandsComponent
import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

trait CommandRunnerComponent {
  this: CosmosDataStoreComponent
    with ServiceManagerComponent
    with PersistentStorageCommandsComponent
    with ProfileCommandsComponent
    with ClusterCommandsComponent =>

  def commandRunner(args: AdminArguments): CommandRunner = new CommandRunner(
    args, store, serviceManager, persistentStorageCommands, profileCommands, clusterCommands)
}
