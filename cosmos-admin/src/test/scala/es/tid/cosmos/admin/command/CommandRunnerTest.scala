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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito._

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.profile.{ProfileCommands, ProfileCommandsComponent}
import es.tid.cosmos.admin.storage.{PersistentStorageCommands, PersistentStorageCommandsComponent}
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.servicemanager.{ServiceManagerComponent, ServiceManager}

class CommandRunnerTest extends FlatSpec with MustMatchers with MockitoSugar {

  class WithArguments(arguments: String*) extends CommandRunnerComponent
      with ServiceManagerComponent with MockCosmosDataStoreComponent with ProfileCommandsComponent
      with PersistentStorageCommandsComponent {
    override val serviceManager = mock[ServiceManager]
    override val persistentStorageCommands = mock[PersistentStorageCommands]
    override val profileCommands = mock[ProfileCommands]
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
