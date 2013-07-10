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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ServiceManager

class CommandRunnerTest extends FlatSpec with MustMatchers with MockitoSugar {

  trait WithServiceManager {
    val serviceManager = mock[ServiceManager]
  }

  it must "support the setup subcommand" in new WithServiceManager {
    val commandRunner = new CommandRunner(Seq("setup"), serviceManager)
    commandRunner.subcommand must be (Some(commandRunner.setup))
  }

  it must "support the persistent-storage setup subcommand" in new WithServiceManager {
    val commandRunner = new CommandRunner(Seq("persistent-storage", "setup"), serviceManager)
    commandRunner.subcommands must equal (
      List(commandRunner.persistent_storage, commandRunner.persistent_storage.setup))
  }

  it must "support the persistent-storage terminate subcommand" in new WithServiceManager {
    val commandRunner = new CommandRunner(Seq("persistent-storage", "terminate"), serviceManager)
    commandRunner.subcommands must equal (
      List(commandRunner.persistent_storage, commandRunner.persistent_storage.terminate))
  }

  it must "support the cluster terminate subcommand" in new WithServiceManager {
    val commandRunner =
      new CommandRunner(Seq("cluster", "terminate", "--clusterid", "cluster1"), serviceManager)
    commandRunner.subcommands must equal (
      List(commandRunner.cluster, commandRunner.cluster.terminate))
  }

  it must "support the profile set-machine-quota subcommand" in new WithServiceManager {
    val commandRunner = new CommandRunner(
      Seq("profile", "set-machine-quota", "--cosmosid", "3", "--limit", "15"), serviceManager)
    commandRunner.subcommands must equal (
      List(commandRunner.profile, commandRunner.profile.set_machine_quota))
  }

  it must "support the profile unset-machine-quota subcommand" in new WithServiceManager {
    val commandRunner =
      new CommandRunner(Seq("profile", "unset-machine-quota", "--cosmosid", "3"), serviceManager)
    commandRunner.subcommands must equal (
      List(commandRunner.profile, commandRunner.profile.unset_machine_quota))
  }
}
