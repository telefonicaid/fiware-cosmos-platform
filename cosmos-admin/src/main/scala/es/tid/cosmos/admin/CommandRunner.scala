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

  private[admin] def run() {
    processCommands(args.subcommands)
  }

  private def processCommands(subcommands: List[ScallopConf]) {
    subcommands.headOption match {
      case Some(args.setup) => setupAll(serviceManager)
      case Some(args.persistentStorage) => processPersistentStorageCommand(subcommands.tail)
      case Some(args.cluster) => processClusterCommand(subcommands.tail)
      case Some(args.profile) => processProfileCommand(subcommands.tail)
      case _ => args.printHelp()
    }
  }

  private def processPersistentStorageCommand(subcommands: List[ScallopConf]) {
    if (subcommands.size == 1) subcommands.headOption match {
      case Some(args.persistentStorage.setup) => PersistentStorage.setup(serviceManager)
      case Some(args.persistentStorage.terminate) => PersistentStorage.terminate(serviceManager)
      case _ => args.persistentStorage.printHelp()
    } else args.persistentStorage.printHelp()
  }

  private def processClusterCommand(subcommands: List[ScallopConf]) {
    if (subcommands.size == 2) subcommands.headOption match {
      case Some(args.cluster.terminate) =>
        Cluster.terminate(serviceManager, ClusterId(args.cluster.terminate.clusterId()))
      case _ => args.cluster.printHelp()
    } else args.cluster.printHelp()
  }

  private def processProfileCommand(subcommands: List[ScallopConf]) = {
    val playDbProfile = new Profile(new PlayDbCosmosProfileDao)
    subcommands.headOption match {
      case Some(args.profile.setMachineQuota) =>
        if (subcommands.size == 3) playDbProfile.setMachineQuota(
          args.profile.setMachineQuota.cosmosid(), args.profile.setMachineQuota.limit())
        else args.profile.printHelp()
      case Some(args.profile.unsetMachineQuota) =>
        if (subcommands.size == 2) playDbProfile.unsetMachineQuota(
          args.profile.setMachineQuota.cosmosid())
        else args.profile.printHelp()
      case _ => args.profile.printHelp()
    }
  }

  /**
   * Performs a setup of all platform components.
   * @param serviceManager The service manager to use.
   * @return Whether the operation succeeded.
   */
  private def setupAll(serviceManager: ServiceManager) = PersistentStorage.setup(serviceManager)
}
