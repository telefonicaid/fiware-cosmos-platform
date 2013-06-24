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

import es.tid.cosmos.admin.setup.SetupCommand
import es.tid.cosmos.servicemanager.ProductionServiceManagerComponent
import es.tid.cosmos.platform.common.ApplicationConfigComponent

class ArgParser(args: Seq[String]) extends ScallopConf(args) {
  val setup = new Subcommand("setup")
}

object ServiceManagerProvider extends ProductionServiceManagerComponent with ApplicationConfigComponent

object Main {
  def main(args: Array[String]) {
    val parsedArgs = new ArgParser(args)
    parsedArgs.subcommand match {
      case Some(command) if command == parsedArgs.setup =>
        new SetupCommand(ServiceManagerProvider.serviceManager).run()
      case Some(_) =>
        throw new Error("Subcommand exists in parser but no associated action is present")
      case None => parsedArgs.printHelp()
    }
  }
}
