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

import org.rogach.scallop.{Subcommand, ScallopConf}

import es.tid.cosmos.api.profile.PlayDbCosmosProfileDao
import es.tid.cosmos.servicemanager.{ClusterId, ServiceManager}

class CommandRunner(args: Seq[String], serviceManager: ServiceManager) extends ScallopConf(args) {

  val setup = new Subcommand("setup")
  val persistent_storage = new Subcommand("persistent-storage") {
    val setup = new Subcommand("setup")
    val terminate = new Subcommand("terminate")
  }
  val cluster = new Subcommand("cluster") {
    val terminate = new Subcommand("terminate") {
      val clusterid = opt[String]("clusterid", required = true)
    }
  }
  val profile = new Subcommand("profile") {
    val set_machine_quota = new Subcommand("set-machine-quota") {
      val cosmosid = opt[Int]("cosmosid", required = true)
      val limit = opt[Int]("limit", required = true)
    }
    val unset_machine_quota = new Subcommand("unset-machine-quota") {
      val cosmosid = opt[Int]("cosmosid", required = true)
    }
  }

  private[admin] def run() {
    parseCommands(subcommands)
  }

  private def parseCommands(subcommands: List[ScallopConf]) = {
    subcommands.head match {
      case `setup` => setupAll(serviceManager)
      case `persistent_storage` =>
        parsePersistentStorageCommand(subcommands.tail)
      case `cluster` => parseClusterCommand(subcommands.tail)
      case `profile` => processProfileCommand(subcommands.tail)
      case _ => printHelp()
    }
  }

  private def parsePersistentStorageCommand(subcommands: List[ScallopConf]) = {
    subcommands.head match {
      case persistent_storage.setup => PersistentStorage.setup(serviceManager)
      case persistent_storage.terminate => PersistentStorage.terminate(serviceManager)
      case _ => printHelp()
    }
  }

  private def parseClusterCommand(subcommands: List[ScallopConf]) = {
    subcommands match {
      case cluster.terminate =>
        Cluster.terminate(serviceManager, ClusterId(cluster.terminate.clusterid()))
      case _ => printHelp()
    }
  }

  private def processProfileCommand(subcommands: List[ScallopConf]) = {
    val playDbProfile = new Profile(new PlayDbCosmosProfileDao)
    subcommands.headOption match {
      case Some(profile.set_machine_quota) =>
        if (subcommands.size == 3) playDbProfile.setMachineQuota(
          profile.set_machine_quota.cosmosid(), profile.set_machine_quota.limit())
        else profile.printHelp()
      case Some(profile.unset_machine_quota) =>
        if (subcommands.size == 2) playDbProfile.unsetMachineQuota(
          profile.set_machine_quota.cosmosid())
        else profile.printHelp()
      case _ => profile.printHelp()
    }
  }

  /**
   * Performs a setup of all platform components.
   * @param serviceManager The service manager to use.
   * @return Whether the operation succeeded.
   */
  private def setupAll(serviceManager: ServiceManager) = {
    PersistentStorage.setup(serviceManager)
  }
}
