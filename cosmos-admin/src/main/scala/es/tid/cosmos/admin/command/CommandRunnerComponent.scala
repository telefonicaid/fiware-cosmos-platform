/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.admin.command

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.cluster.ClusterCommandsComponent
import es.tid.cosmos.admin.groups.GroupCommandsComponent
import es.tid.cosmos.admin.profile.ProfileCommandsComponent
import es.tid.cosmos.admin.storage.PersistentStorageCommandsComponent
import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent

trait CommandRunnerComponent {
  this: CosmosDataStoreComponent
    with PersistentStorageCommandsComponent
    with ProfileCommandsComponent
    with GroupCommandsComponent
    with ClusterCommandsComponent =>

  def commandRunner(args: AdminArguments): CommandRunner = new CommandRunner(
    args, store, persistentStorageCommands, profileCommands, groupCommands, clusterCommands)
}
