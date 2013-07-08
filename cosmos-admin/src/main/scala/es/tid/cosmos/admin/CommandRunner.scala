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

  private[admin] def run() {
    parseCommands(subcommands)
  }

  private def parseCommands(subcommands: List[ScallopConf]) = {
    subcommands.head match {
      case `setup` => setupAll(serviceManager)
      case `persistent_storage` =>
        parsePersistentStorageCommand(subcommands.tail)
      case `cluster` => parseClusterCommand(subcommands.tail)
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

  /**
   * Performs a setup of all platform components.
   * @param serviceManager The service manager to use.
   * @return Whether the operation succeeded.
   */
  private def setupAll(serviceManager: ServiceManager) = {
    PersistentStorage.setup(serviceManager)
  }
}
