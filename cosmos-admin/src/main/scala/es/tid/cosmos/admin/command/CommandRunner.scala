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

import scala.language.reflectiveCalls

import org.rogach.scallop.ScallopConf

import es.tid.cosmos.admin.{Profile, Cluster, PersistentStorage}
import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.groups.GroupCommands
import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.ClusterId

class CommandRunner(
    args: AdminArguments,
    store: CosmosDataStore,
    serviceManager: => ServiceManager) {

  /** Executes an administration command.
    *
    * @return Command result
    */
  def run(): CommandResult = processCommands(args.subcommands)

  private def processCommands(subCommands: List[ScallopConf]): CommandResult =
    subCommands.headOption match {
      case Some(args.setup) => CommandResult.fromBlock(setupAll(serviceManager))
      case Some(args.persistentStorage) => processPersistentStorageCommand(subCommands.tail)
      case Some(args.cluster) => processClusterCommand(subCommands.tail)
      case Some(args.profile) => processProfileCommand(subCommands.tail)
      case Some(args.group) => processGroupCommand(subCommands.tail)
      case _ => help(args)
    }

  private def processPersistentStorageCommand(subCommands: List[ScallopConf]): CommandResult =
    subCommands.headOption match {
      case Some(args.persistentStorage.setup) =>
        CommandResult.fromBlock(PersistentStorage.setup(serviceManager))
      case Some(args.persistentStorage.terminate) =>
        CommandResult.fromBlock(PersistentStorage.terminate(serviceManager))
      case _ => help(args.persistentStorage)
    }

  private def processClusterCommand(subCommands: List[ScallopConf]) = subCommands.headOption match {
      case Some(args.cluster.terminate) =>
        CommandResult.fromBlock(
          Cluster.terminate(serviceManager, ClusterId(args.cluster.terminate.clusterId()))
        )
      case _ => help(args.cluster)
  }

  private def processProfileCommand(subCommands: List[ScallopConf]) = {
    val playDbProfile = new Profile(store)
    subCommands.headOption match {
      case Some(args.profile.setMachineQuota) =>
        CommandResult.fromBlock(playDbProfile.setMachineQuota(
          args.profile.setMachineQuota.handle(), args.profile.setMachineQuota.limit())
        )
      case Some(args.profile.removeMachineQuota) =>
        CommandResult.fromBlock(playDbProfile.removeMachineQuota(
          args.profile.setMachineQuota.handle()
        ))
      case Some(args.profile.enableCapability) =>
        CommandResult.fromBlock(playDbProfile.enableCapability(
          args.profile.enableCapability.handle(),
          args.profile.enableCapability.capability()
        ))
      case Some(args.profile.disableCapability) =>
        CommandResult.fromBlock(playDbProfile.disableCapability(
          args.profile.disableCapability.handle(),
          args.profile.disableCapability.capability()
        ))
      case Some(args.profile.setGroup) =>
        CommandResult.fromBlock(playDbProfile.setGroup(
          args.profile.setGroup.handle(), args.profile.setGroup.group()
        ))
      case Some(args.profile.removeGroup) =>
        CommandResult.fromBlock(playDbProfile.removeGroup(
          args.profile.setGroup.handle()
        ))
      case Some(args.profile.list) => CommandResult.fromBlockWithOutput(playDbProfile.list)
      case _ => help(args.profile)
    }
  }

  private def processGroupCommand(subCommands: List[ScallopConf]) = {
    val groups = new GroupCommands(store, serviceManager)
    subCommands.headOption match {
      case Some(args.group.create) => CommandResult.fromBlock(groups.create(
        args.group.create.name(), args.group.create.minQuota()))
      case Some(args.group.list) => CommandResult.fromBlockWithOutput(groups.list)
      case Some(args.group.delete) => groups.delete(args.group.delete.name())
      case Some(args.group.setMinQuota) => CommandResult.fromBlock(groups.setMinQuota(
        args.group.setMinQuota.name(), args.group.setMinQuota.quota()))
      case _ => help(args.group)
    }
  }

  /** Performs a setup of all platform components.
    * @param serviceManager The service manager to use.
    * @return Whether the operation succeeded.
    */
  private def setupAll(serviceManager: ServiceManager) = PersistentStorage.setup(serviceManager)

  private def help(conf: ScallopConf) = {
    conf.printHelp()
    CommandResult.error("Invalid arguments", CommandResult.InvalidArgsStatus)
  }
}
