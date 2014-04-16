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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito._

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.cluster.{ClusterCommands, ClusterCommandsComponent}
import es.tid.cosmos.admin.groups.{GroupCommands, GroupCommandsComponent}
import es.tid.cosmos.admin.profile.{ProfileCommands, ProfileCommandsComponent}
import es.tid.cosmos.admin.storage.{PersistentStorageCommands, PersistentStorageCommandsComponent}
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent

class CommandRunnerTest extends FlatSpec with MustMatchers with MockitoSugar {

  class WithArguments(arguments: String*) extends CommandRunnerComponent
      with MockCosmosDataStoreComponent
      with ProfileCommandsComponent
      with GroupCommandsComponent
      with PersistentStorageCommandsComponent with ClusterCommandsComponent {
    override val persistentStorageCommands = mock[PersistentStorageCommands]
    override val profileCommands = mock[ProfileCommands]
    override val groupCommands = mock[GroupCommands]
    override val clusterCommands = mock[ClusterCommands]
    val runner = commandRunner(new AdminArguments(arguments.toSeq))
  }

  "A command runner" must "exit with non-zero status for invalid arguments" in
    new WithArguments() {
      runner.run().exitStatus must not be 0
    }

  it must "exit with non-zero status when setup fails" in new WithArguments("setup") {
    given(persistentStorageCommands.setup()).willReturn(CommandResult.error())
    runner.run().exitStatus must not be 0
  }

  it must "exit with zero status when everything goes OK" in new WithArguments("setup") {
    given(persistentStorageCommands.setup()).willReturn(CommandResult.success())
    runner.run().exitStatus must be (0)
  }

  it must "work with multi-subcommand arguments" in new WithArguments("persistent-storage", "setup") {
    given(persistentStorageCommands.setup()).willReturn(CommandResult.success())
    runner.run().exitStatus must be (0)
  }
}
