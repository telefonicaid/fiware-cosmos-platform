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

import scala.language.reflectiveCalls

import org.rogach.scallop.ScallopConf

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.api.profile.dao.sql.PlayDbDataStoreComponent
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.ClusterId

class CommandRunner(args: AdminArguments, serviceManager: => ServiceManager)
  extends PlayDbDataStoreComponent {

  /** Executes an administration command.
    *
    * @return Exit status
    */
  def run(): Int = processCommands(args.subcommands)

  private def processCommands(subCommands: List[ScallopConf]): Int =
    subCommands.headOption match {
      case Some(args.setup) => tryCommand(setupAll(serviceManager))
      case Some(args.persistentStorage) => processPersistentStorageCommand(subCommands.tail)
      case Some(args.cluster) => processClusterCommand(subCommands.tail)
      case Some(args.profile) => processProfileCommand(subCommands.tail)
      case Some(args.group) => processGroupCommand(subCommands.tail)
      case _ => help(args)
    }

  private def processPersistentStorageCommand(subCommands: List[ScallopConf]) =
    subCommands.headOption match {
      case Some(args.persistentStorage.setup) => tryCommand(PersistentStorage.setup(serviceManager))
      case Some(args.persistentStorage.terminate) =>
        tryCommand(PersistentStorage.terminate(serviceManager))
      case _ => help(args.persistentStorage)
    }

  private def processClusterCommand(subCommands: List[ScallopConf]) = subCommands.headOption match {
      case Some(args.cluster.terminate) =>
        tryCommand(Cluster.terminate(serviceManager, ClusterId(args.cluster.terminate.clusterId())))
      case _ => help(args.cluster)
  }

  private def processProfileCommand(subCommands: List[ScallopConf]) = {
    val playDbProfile = new Profile(store)
    subCommands.headOption match {
      case Some(args.profile.setMachineQuota) => tryCommand(playDbProfile.setMachineQuota(
          args.profile.setMachineQuota.handle(), args.profile.setMachineQuota.limit()))
      case Some(args.profile.removeMachineQuota) => tryCommand(playDbProfile.removeMachineQuota(
          args.profile.setMachineQuota.handle()))
      case Some(args.profile.enableCapability) => tryCommand(playDbProfile.enableCapability(
        args.profile.enableCapability.handle(),
        args.profile.enableCapability.capability()
      ))
      case Some(args.profile.disableCapability) => tryCommand(playDbProfile.disableCapability(
        args.profile.disableCapability.handle(),
        args.profile.disableCapability.capability()
      ))
      case Some(args.profile.setGroup) => tryCommand(playDbProfile.setGroup(
        args.profile.setGroup.handle(), args.profile.setGroup.group()
      ))
      case Some(args.profile.removeGroup) => tryCommand(playDbProfile.removeGroup(
        args.profile.setGroup.handle()
      ))
      case Some(args.profile.list) => tryCommandWithOutput(playDbProfile.list)
      case _ => help(args.profile)
    }
  }

  private def processGroupCommand(subCommands: List[ScallopConf]) = {
    val groups = new Groups(store, serviceManager)
    subCommands.headOption match {
      case Some(args.group.create) => tryCommand(groups.create(
        args.group.create.name(), args.group.create.minQuota()))
      case Some(args.group.list) => tryCommandWithOutput(groups.list)
      case Some(args.group.delete) => tryCommand(groups.delete(args.group.delete.name()))
      case Some(args.group.setMinQuota) => tryCommand(groups.setMinQuota(
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
    CommandRunner.InvalidArgsStatus
  }

  private def tryCommand(block: => Boolean) = try {
    if (block) CommandRunner.SuccessStatus
    else CommandRunner.ExecutionErrorStatus
  } catch {
    case ex: Throwable =>
      ex.printStackTrace()
      CommandRunner.ExecutionErrorStatus
  }

  private def tryCommandWithOutput(block: => String) = try {
    println(block)
    CommandRunner.SuccessStatus
  } catch {
    case ex: Throwable =>
      ex.printStackTrace()
      CommandRunner.ExecutionErrorStatus
  }
}

object CommandRunner {
  val SuccessStatus = 0
  val InvalidArgsStatus = 1
  val ExecutionErrorStatus = -1
}
