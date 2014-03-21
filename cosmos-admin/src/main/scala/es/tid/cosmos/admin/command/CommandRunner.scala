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

import es.tid.cosmos.admin.ClusterCommands
import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.groups.GroupCommands
import es.tid.cosmos.admin.profile.ProfileCommands
import es.tid.cosmos.admin.storage.PersistentStorageCommands
import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.ClusterId

class CommandRunner(
    args: AdminArguments,
    store: CosmosDataStore,
    serviceManager: ServiceManager,
    storageCommands: PersistentStorageCommands,
    profileCommands: ProfileCommands) {

  /** Executes an administration command.
    *
    * @return Command result
    */
  def run(): CommandResult = processCommands(args.subcommands)

  private def processCommands(subCommands: List[ScallopConf]): CommandResult =
    subCommands.headOption match {
      case Some(args.setup) => setupAll()
      case Some(args.persistentStorage) => processPersistentStorageCommand(subCommands.tail)
      case Some(args.cluster) => processClusterCommand(subCommands.tail)
      case Some(args.profile) => processProfileCommand(subCommands.tail)
      case Some(args.group) => processGroupCommand(subCommands.tail)
      case _ => help(args)
    }

  private def processPersistentStorageCommand(subCommands: List[ScallopConf]): CommandResult =
    subCommands.headOption match {
      case Some(args.persistentStorage.setup) => storageCommands.setup()
      case Some(args.persistentStorage.terminate) => storageCommands.terminate()
      case _ => help(args.persistentStorage)
    }

  private def processClusterCommand(subCommands: List[ScallopConf]) = subCommands.headOption match {
    case Some(args.cluster.terminate) =>
      ClusterCommands.terminate(serviceManager, ClusterId(args.cluster.terminate.clusterId()))
    case _ => help(args.cluster)
  }

  private def processProfileCommand(subCommands: List[ScallopConf]) = {
    subCommands.headOption match {
      case Some(args.profile.setMachineQuota) => profileCommands.quota.set(
        args.profile.setMachineQuota.handle(), args.profile.setMachineQuota.limit()
      )
      case Some(args.profile.removeMachineQuota) =>
        profileCommands.quota.remove(args.profile.setMachineQuota.handle())
      case Some(args.profile.enableCapability) => profileCommands.capability.enable(
        args.profile.enableCapability.handle(),
        args.profile.enableCapability.capability()
      )
      case Some(args.profile.disableCapability) => profileCommands.capability.disable(
        args.profile.disableCapability.handle(),
        args.profile.disableCapability.capability()
      )
      case Some(args.profile.setGroup) =>
        profileCommands.group.set(args.profile.setGroup.handle(), args.profile.setGroup.group())
      case Some(args.profile.removeGroup) =>
        profileCommands.group.remove(args.profile.setGroup.handle())
      case Some(args.profile.list) => profileCommands.list()
      case _ => help(args.profile)
    }
  }

  private def processGroupCommand(subCommands: List[ScallopConf]) = {
    val groups = new GroupCommands(store, serviceManager)
    subCommands.headOption match {
      case Some(args.group.create) =>
        groups.create(args.group.create.name(), args.group.create.minQuota())
      case Some(args.group.list) => groups.list()
      case Some(args.group.delete) => groups.delete(args.group.delete.name())
      case Some(args.group.setMinQuota) =>
        groups.setMinQuota(args.group.setMinQuota.name(), args.group.setMinQuota.quota())
      case _ => help(args.group)
    }
  }

  /** Performs a setup of all platform components.
    * @return Whether the operation succeeded.
    */
  private def setupAll() = storageCommands.setup()

  private def help(conf: ScallopConf) = {
    conf.printHelp()
    CommandResult.error("Invalid arguments", CommandResult.InvalidArgsStatus)
  }
}
