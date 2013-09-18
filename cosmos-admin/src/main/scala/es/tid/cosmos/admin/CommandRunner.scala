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

class CommandRunner(args: AdminArguments, serviceManager: ServiceManager) {

  /**
   * Executes an administration command.
   *
   * @return Exit status
   */
  def run(): Int = processCommands(args.subcommands)

  private def processCommands(subcommands: List[ScallopConf]): Int =
    subcommands.headOption match {
      case Some(args.setup) => tryCommand(setupAll(serviceManager))
      case Some(args.persistentStorage) => processPersistentStorageCommand(subcommands.tail)
      case Some(args.cluster) => processClusterCommand(subcommands.tail)
      case Some(args.profile) => processProfileCommand(subcommands.tail)
      case _ => help(args)
    }

  private def processPersistentStorageCommand(subcommands: List[ScallopConf]) =
    if (subcommands.size == 1) subcommands.headOption match {
      case Some(args.persistentStorage.setup) => tryCommand(PersistentStorage.setup(serviceManager))
      case Some(args.persistentStorage.terminate) => tryCommand(PersistentStorage.terminate(serviceManager))
      case _ => help(args.persistentStorage)
    } else help(args.persistentStorage)

  private def processClusterCommand(subcommands: List[ScallopConf]) =
    if (subcommands.size == 2) subcommands.headOption match {
      case Some(args.cluster.terminate) =>
        tryCommand(Cluster.terminate(serviceManager, ClusterId(args.cluster.terminate.clusterId())))
      case _ => help(args.cluster)
    } else help(args.cluster)

  private def processProfileCommand(subcommands: List[ScallopConf]) = {
    val playDbProfile = new Profile(new PlayDbCosmosProfileDao)
    subcommands.headOption match {
      case Some(args.profile.setMachineQuota) =>
        if (subcommands.size == 3) tryCommand(playDbProfile.setMachineQuota(
          args.profile.setMachineQuota.cosmosid(), args.profile.setMachineQuota.limit()))
        else help(args.profile)
      case Some(args.profile.unsetMachineQuota) =>
        if (subcommands.size == 2) tryCommand(playDbProfile.unsetMachineQuota(
          args.profile.setMachineQuota.cosmosid()))
        else help(args.profile)
      case _ => help(args.profile)
    }
  }

  /**
   * Performs a setup of all platform components.
   * @param serviceManager The service manager to use.
   * @return Whether the operation succeeded.
   */
  private def setupAll(serviceManager: ServiceManager) = PersistentStorage.setup(serviceManager)

  private def help(conf: ScallopConf) = {
    conf.printHelp()
    CommandRunner.InvalidArgsStatus
  }

  private def tryCommand(block: => Boolean) = try {
    if (block) CommandRunner.SuccessStatus
    else CommandRunner.ExecutionErrorStatus
  } catch {
    case ex: Throwable => {
      ex.printStackTrace()
      CommandRunner.ExecutionErrorStatus
    }
  }
}

object CommandRunner {
  val SuccessStatus = 0
  val InvalidArgsStatus = 1
  val ExecutionErrorStatus = -1
}
