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

package es.tid.cosmos.admin

import _root_.play.api.Play

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.cluster.DefaultClusterCommandsComponent
import es.tid.cosmos.admin.groups.DefaultGroupCommandsComponent
import es.tid.cosmos.admin.command.CommandRunnerComponent
import es.tid.cosmos.admin.play.DataAccessApplicationComponent
import es.tid.cosmos.admin.profile.DefaultProfileCommandsComponent
import es.tid.cosmos.admin.storage.DefaultPersistentStorageCommandsComponent
import es.tid.cosmos.api.profile.dao.sql.PlayDbDataStoreComponent
import es.tid.cosmos.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.production.ProductionServiceManagerComponent

object Main extends CommandRunnerComponent
  with DefaultPersistentStorageCommandsComponent
  with DefaultProfileCommandsComponent
  with DefaultGroupCommandsComponent
  with DefaultClusterCommandsComponent
  with DataAccessApplicationComponent
  with ProductionServiceManagerComponent
  with PlayDbDataStoreComponent
  with ApplicationConfigComponent {

  def main(args: Array[String]): Unit = {
    Play.start(playApplication)
    val result = try {
      commandRunner(new AdminArguments(args)).run()
    } finally {
      Play.stop()
    }
    println(result.message)
    System.exit(result.exitStatus)
  }
}
